/***********************************************************************
 * Module:  CTPOTCTradeWorkerProcessorBase.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class CTPOTCTradeWorkerProcessorBase
 ***********************************************************************/

#include "CTPOTCTradeWorkerProcessorBase.h"
#include "CTPOTCTradeSAProcessor.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/CTPConstant.h"
#include "../CTPServer/CTPMapping.h"
#include "../dataobject/TradeRecordDO.h"
#include "../common/Attribute_Key.h"
#include "../systemsettings/AppContext.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../bizutility/ExchangeRouterTable.h"
#include "../bizutility/PositionPortfolioMap.h"

#include "../databaseop/VersionDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/TradeDAO.h"
#include "../ordermanager/OrderSeqGen.h"
#include "../ordermanager/OrderReqCache.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"


CTPOTCTradeWorkerProcessorBase::CTPOTCTradeWorkerProcessorBase(
	IServerContext* pServerCtx,
	const IPricingDataContext_Ptr& pricingDataCtx,
	const IUserPositionContext_Ptr& positionCtx)
	:OTCTradeWorkerProcessor(pricingDataCtx),
	_otcOrderMgr(pricingDataCtx, this),
	_autoOrderMgr(pricingDataCtx, positionCtx),
	_hedgeOrderMgr(pricingDataCtx, positionCtx)
{
	_autoOrderWorker = std::async(std::launch::async, &CTPOTCTradeWorkerProcessorBase::AutoOrderWorker, this);
	_hedgeOrderWorker = std::async(std::launch::async, &CTPOTCTradeWorkerProcessorBase::HedgeOrderWorker, this);
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradeWorkerProcessorBase::~CTPOTCTradeWorkerProcessorBase()
{
	Dispose();		

	LOG_DEBUG << __FUNCTION__;
}

bool CTPOTCTradeWorkerProcessorBase::Dispose(void)
{
	Closing() = true;

	if (_autoOrderWorker.valid())
		_autoOrderWorker.wait();

	if (_hedgeOrderWorker.valid())
		_hedgeOrderWorker.wait();

	_mdWorkerProcessor.reset();

	return OTCTradeProcessor::Dispose();
}


void CTPOTCTradeWorkerProcessorBase::AutoOrderWorker()
{
	while (!Closing())
	{
		std::pair<UserContractKey, IOrderAPI_Ptr> strategyKey;
		if (_autoOrderQueue.pop(strategyKey) && strategyKey.second)
		{
			StrategyContractDO_Ptr strategy_ptr;
			if (PricingDataContext()->GetStrategyMap()->find(strategyKey.first, strategy_ptr))
			{
				_autoOrderMgr.TradeByStrategy(*strategy_ptr, strategyKey.second.get());
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void CTPOTCTradeWorkerProcessorBase::HedgeOrderWorker()
{
	while (!Closing())
	{
		std::pair<PortfolioKey, IOrderAPI_Ptr> portfolioKey;
		if (_hedgeOrderQueue.pop(portfolioKey) && portfolioKey.second)
		{
			auto status = _hedgeOrderMgr.Hedge(portfolioKey.first, portfolioKey.second.get());
			if (status == HedgeOrderManager::Waiting)
			{
				_waitingHedgeQueue.emplace(portfolioKey);
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			while (_waitingHedgeQueue.pop(portfolioKey))
			{
				_hedgeOrderQueue.emplace(portfolioKey);
			}
		}
	}
}

void CTPOTCTradeWorkerProcessorBase::TriggerHedgeOrderUpdating(const PortfolioKey& portfolioKey, const IOrderAPI_Ptr& orderAPI_ptr)
{
	if (orderAPI_ptr)
	{
		_hedgeOrderQueue.emplace(std::make_pair(portfolioKey, orderAPI_ptr));
	}
	else
	{
		GetUserSessionContainer()->forfirst(portfolioKey.UserID(), [this, &portfolioKey](const IMessageSession_Ptr& msgSession)
		{
			if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
				_hedgeOrderQueue.emplace(std::make_pair(portfolioKey, otcTradeProc_Ptr));
		});
	}
}

void CTPOTCTradeWorkerProcessorBase::TriggerAutoOrderUpdating(const UserContractKey& strategyKey, const IOrderAPI_Ptr& orderAPI_ptr)
{
	if (orderAPI_ptr)
	{
		_autoOrderQueue.emplace(std::make_pair(strategyKey, orderAPI_ptr));
	}
	else
	{
		GetUserSessionContainer()->forfirst(strategyKey.UserID(), [this, &strategyKey](const IMessageSession_Ptr& msgSession)
		{
			if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
				_autoOrderQueue.emplace(std::make_pair(strategyKey, otcTradeProc_Ptr));
		});
	}
}

std::shared_ptr<CTPOTCWorkerProcessor> CTPOTCTradeWorkerProcessorBase::GetMDWorkerProcessor()
{
	return _mdWorkerProcessor;
}

void CTPOTCTradeWorkerProcessorBase::SetMDWorkerProcessor(const std::shared_ptr<CTPOTCWorkerProcessor>& workerProcessor)
{
	_mdWorkerProcessor = workerProcessor;
}

OTCOrderManager& CTPOTCTradeWorkerProcessorBase::GetOTCOrderManager(void)
{
	return _otcOrderMgr;
}

AutoOrderManager& CTPOTCTradeWorkerProcessorBase::GetAutoOrderManager(void)
{
	return _autoOrderMgr;
}

HedgeOrderManager& CTPOTCTradeWorkerProcessorBase::GetHedgeOrderManager(void)
{
	return _hedgeOrderMgr;
}

void CTPOTCTradeWorkerProcessorBase::OnNewManualTrade(const TradeRecordDO & tradeDO)
{
	GetUserTradeContext().InsertTrade(tradeDO.UserID(), tradeDO);

	auto tradeDO_Ptr = std::make_shared<TradeRecordDO>(tradeDO);

	if (!GetUserPositionContext()->ContainsTrade(tradeDO.TradeID128()))
	{
		int multiplier = 1;
		if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(tradeDO.InstrumentID()))
		{
			multiplier = pContractInfo->VolumeMultiple;
		}

		GetUserPositionContext()->UpsertPosition(tradeDO.UserID(), tradeDO_Ptr, multiplier);
	}
}