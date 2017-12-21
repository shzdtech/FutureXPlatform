/***********************************************************************
 * Module:  CTPOTCTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class CTPOTCTradeWorkerProcessor
 ***********************************************************************/

#include "CTPOTCTradeWorkerProcessor.h"
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
#include "../ordermanager/OrderPortfolioCache.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"


CTPOTCTradeWorkerProcessor::CTPOTCTradeWorkerProcessor(
	IServerContext* pServerCtx,
	const IPricingDataContext_Ptr& pricingDataCtx,
	const IUserPositionContext_Ptr& positionCtx)
	: OTCTradeWorkerProcessor(pricingDataCtx),
	CTPTradeWorkerSAProcessor(pServerCtx, pricingDataCtx, positionCtx),
	_otcOrderMgr(pricingDataCtx, this),
	_autoOrderMgr(pricingDataCtx, _userPositionCtx_Ptr),
	_hedgeOrderMgr(pricingDataCtx, _userPositionCtx_Ptr)
{
	_logTrades = false;

	_autoOrderWorker = std::async(std::launch::async, &CTPOTCTradeWorkerProcessor::AutoOrderWorker, this);
	_hedgeOrderWorker = std::async(std::launch::async, &CTPOTCTradeWorkerProcessor::HedgeOrderWorker, this);
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradeWorkerProcessor::~CTPOTCTradeWorkerProcessor()
{
	if (!_closing)
		Dispose();

	LOG_DEBUG << __FUNCTION__;
}

bool CTPOTCTradeWorkerProcessor::Dispose(void)
{
	_closing = true;

	if (_autoOrderWorker.valid())
		_autoOrderWorker.wait();

	if (_hedgeOrderWorker.valid())
		_hedgeOrderWorker.wait();

	_mdWorkerProcessor.reset();

	return OTCTradeProcessor::Dispose();
}


void CTPOTCTradeWorkerProcessor::AutoOrderWorker()
{
	while (!_closing)
	{
		std::pair<UserContractKey, OTCTradeProcessor_Ptr> strategyKey;
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

void CTPOTCTradeWorkerProcessor::HedgeOrderWorker()
{
	while (!_closing)
	{
		std::pair<PortfolioKey, OTCTradeProcessor_Ptr> portfolioKey;
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

void CTPOTCTradeWorkerProcessor::TriggerHedgeOrderUpdating(const PortfolioKey& portfolioKey, const OTCTradeProcessor_Ptr& processor_ptr)
{
	if (processor_ptr)
	{
		_hedgeOrderQueue.emplace(std::make_pair(portfolioKey, processor_ptr));
	}
	else
	{
		_userSessionCtn_Ptr->forfirst(portfolioKey.UserID(), [this, &portfolioKey](const IMessageSession_Ptr& msgSession)
		{
			if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
				_hedgeOrderQueue.emplace(std::make_pair(portfolioKey, otcTradeProc_Ptr));
		});
	}
}

void CTPOTCTradeWorkerProcessor::TriggerAutoOrderUpdating(const UserContractKey& strategyKey, const OTCTradeProcessor_Ptr& processor_ptr)
{
	if (processor_ptr)
	{
		_autoOrderQueue.emplace(std::make_pair(strategyKey, processor_ptr));
	}
	else
	{
		_userSessionCtn_Ptr->forfirst(strategyKey.UserID(), [this, &strategyKey](const IMessageSession_Ptr& msgSession)
		{
			if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
				_autoOrderQueue.emplace(std::make_pair(strategyKey, otcTradeProc_Ptr));
		});
	}
}

std::shared_ptr<CTPOTCWorkerProcessor> CTPOTCTradeWorkerProcessor::GetMDWorkerProcessor()
{
	return _mdWorkerProcessor;
}

void CTPOTCTradeWorkerProcessor::SetMDWorkerProcessor(const std::shared_ptr<CTPOTCWorkerProcessor>& workerProcessor)
{
	_mdWorkerProcessor = workerProcessor;
}

void CTPOTCTradeWorkerProcessor::OnTraded(const TradeRecordDO_Ptr& tradeDO)
{
	DispatchUserMessage(MSG_ID_TRADE_RTN, 0, tradeDO->UserID(), tradeDO);

	if (auto position_ptr = GetOTCOrderManager().GetPositionContext().GetPosition(tradeDO->UserID(), tradeDO->PortfolioID(), *tradeDO))
	{
		DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, tradeDO->UserID(), position_ptr);
	}
}


OrderDO_Ptr CTPOTCTradeWorkerProcessor::CreateOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(orderReq.UserID(), [this, &orderReq, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CreateOrder(orderReq);
	});

	return ret;
}


OrderDO_Ptr CTPOTCTradeWorkerProcessor::CancelOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(orderReq.UserID(), [this, &orderReq, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelOrder(orderReq);
	});

	return ret;
}


OrderDO_Ptr CTPOTCTradeWorkerProcessor::CancelAutoOrder(const UserContractKey& userContractKey)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(userContractKey.UserID(), [this, &userContractKey, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelAutoOrder(userContractKey);
	});

	return ret;
}

OrderDO_Ptr CTPOTCTradeWorkerProcessor::CancelHedgeOrder(const PortfolioKey& portfolioKey)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(portfolioKey.UserID(), [this, &portfolioKey, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelHedgeOrder(portfolioKey);
	});

	return ret;
}


uint32_t CTPOTCTradeWorkerProcessor::GetSessionId(void)
{
	return _systemUser.getSessionId();
}

OTCOrderManager& CTPOTCTradeWorkerProcessor::GetOTCOrderManager(void)
{
	return _otcOrderMgr;
}

AutoOrderManager& CTPOTCTradeWorkerProcessor::GetAutoOrderManager(void)
{
	return _autoOrderMgr;
}

HedgeOrderManager& CTPOTCTradeWorkerProcessor::GetHedgeOrderManager(void)
{
	return _hedgeOrderMgr;
}

UserOrderContext& CTPOTCTradeWorkerProcessor::GetExchangeOrderContext()
{
	return _userOrderCtx;
}

void CTPOTCTradeWorkerProcessor::OnNewManualTrade(const TradeRecordDO & tradeDO)
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


//CTP API

///登录请求响应
void CTPOTCTradeWorkerProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CTPTradeWorkerSAProcessor::OnRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);
	if (!CTPUtility::HasError(pRspInfo))
	{
		// _autoOrderMgr.CancelUserOrders(_systemUser.getUserId(), this);
	}
}

void CTPOTCTradeWorkerProcessor::OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void CTPOTCTradeWorkerProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void CTPOTCTradeWorkerProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo)
{
}

void CTPOTCTradeWorkerProcessor::OnErrRtnOrderAction(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRspInfo)
{
}

void CTPOTCTradeWorkerProcessor::OnRtnOrder(CThostFtdcOrderField * pOrder)
{
}


void CTPOTCTradeWorkerProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
}

void CTPOTCTradeWorkerProcessor::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast)
		DataLoadMask |= CTPProcessor::POSITION_DATA_LOADED;
}