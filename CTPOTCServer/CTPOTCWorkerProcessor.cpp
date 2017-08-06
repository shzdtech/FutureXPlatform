/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#include "CTPOTCWorkerProcessor.h"

#include "../CTPServer/CTPUtility.h"
#include "../litelogger/LiteLogger.h"

#include "../CTPServer/CTPConstant.h"

#include "../common/Attribute_Key.h"

#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../systemsettings/AppContext.h"
#include "../dataobject/EnumTypes.h"
#include "../pricingengine/PricingUtility.h"

#include "../databaseop/OTCOrderDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"
#include "../databaseop/ModelParamsDAO.h"
#include "../pricingengine/PricingAlgorithmManager.h"
#include "../pricingengine/ModelAlgorithmManager.h"

#include "../bizutility/ContractCache.h"
#include "../bizutility/StrategyModelCache.h"
#include "../bizutility/ExchangeRouterTable.h"

#include "../ordermanager/OrderSeqGen.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of CTPOTCWorkerProcessor::CTPOTCWorkerProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(IServerContext* pServerCtx,
	const std::shared_ptr<CTPOTCTradeProcessor>& otcTradeProcessorPtr) :
	CTPMarketDataSAProcessor(pServerCtx),
	OTCWorkerProcessor(otcTradeProcessorPtr->PricingDataContext()),
	_ctpOtcTradeProcessorPtr(otcTradeProcessorPtr)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
// Purpose:    Implementation of CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCWorkerProcessor::~CTPOTCWorkerProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

void CTPOTCWorkerProcessor::setMessageSession(const IMessageSession_Ptr& msgSession_ptr)
{
	CTPMarketDataSAProcessor::setMessageSession(msgSession_ptr);
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->setMessageSession(msgSession_ptr);
}

void CTPOTCWorkerProcessor::setServiceLocator(const IMessageServiceLocator_Ptr& svcLct_Ptr)
{
	CTPMarketDataSAProcessor::setServiceLocator(svcLct_Ptr);
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->setServiceLocator(svcLct_Ptr);
}

bool CTPOTCWorkerProcessor::OnSessionClosing(void)
{
	_closing = true;

	if (_initializer.valid())
		_initializer.wait();

	GetOTCTradeProcessor()->Dispose();

	return true;
}


void CTPOTCWorkerProcessor::Initialize(IServerContext* serverCtx)
{
	OTCWorkerProcessor::Initialize();

	CTPMarketDataSAProcessor::Initialize(serverCtx);
}


int CTPOTCWorkerProcessor::SubscribeMarketData(const ContractKey& contractId)
{
	int ret = -1;
	char* contract[] = { const_cast<char*>(contractId.InstrumentID().data()) };
	auto mdApiProxy = _rawAPI->MdAPIProxy();
	if(mdApiProxy)
		ret = mdApiProxy->get()->SubscribeMarketData(contract, 1);
	return ret;
}

void CTPOTCWorkerProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	((CTPOTCTradeProcessor*)GetOTCTradeProcessor())->RegisterLoggedSession(sessionPtr);

	if (!sessionPtr->getUserInfo().getTradingDay())
	{
		auto& userInfo = sessionPtr->getUserInfo();
		userInfo.setTradingDay(_systemUser.getTradingDay());
	}
}

ProductType CTPOTCWorkerProcessor::GetContractProductType() const
{
	return ProductType::PRODUCT_OTC;
}

const std::vector<ProductType>& CTPOTCWorkerProcessor::GetStrategyProductTypes() const
{
	static const std::vector<ProductType> productTypes = { ProductType::PRODUCT_OTC };
	return productTypes;
}


OTCTradeProcessor * CTPOTCWorkerProcessor::GetOTCTradeProcessor()
{
	return _ctpOtcTradeProcessorPtr.get();
}

CTPOTCTradeProcessor * CTPOTCWorkerProcessor::GetCTPOTCTradeProcessor()
{
	return _ctpOtcTradeProcessorPtr.get();
}




//CTP APIs

void CTPOTCWorkerProcessor::OnFrontConnected()
{
	_isConnected = true;

	auto loginTime = _systemUser.getLoginTime();
	auto now = std::time(nullptr);
	if (_isLogged && (now - loginTime) > 600)
	{
		LoginSystemUser();
	}

	LOG_INFO << getServerContext()->getServerUri() << ": Front Connected.";
}


void CTPOTCWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	if (!_closing)
	{
		auto mdMap = PricingDataContext()->GetMarketDataMap();
		bool updated = false;
		mdMap->update_fn(pDepthMarketData->InstrumentID, [pDepthMarketData, &updated](MarketDataDO& mdo)
		{
			if (mdo.Bid().Price != pDepthMarketData->BidPrice1 ||
				mdo.Ask().Price != pDepthMarketData->AskPrice1 ||
				mdo.Bid().Volume != pDepthMarketData->BidVolume1 ||
				mdo.Ask().Volume != pDepthMarketData->AskVolume1)
			{
				mdo.Bid().Price = pDepthMarketData->BidPrice1;
				mdo.Ask().Price = pDepthMarketData->AskPrice1;
				mdo.Bid().Volume = pDepthMarketData->BidVolume1;
				mdo.Ask().Volume = pDepthMarketData->AskVolume1;

				//mdo.PreClosePrice = pDepthMarketData->PreClosePrice;
				//mdo.OpenPrice = pDepthMarketData->OpenPrice;
				//mdo.HighestPrice = pDepthMarketData->HighestPrice;
				//mdo.LowestPrice = pDepthMarketData->LowestPrice;
				//mdo.Volume = pDepthMarketData->Volume;
				//mdo.Turnover = pDepthMarketData->Turnover;
				//mdo.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
				//mdo.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
				//mdo.PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
				//mdo.SettlementPrice = pDepthMarketData->SettlementPrice;

				updated = true;
			}
		});

		if (updated)
		{
			MarketDataDO mdo;
			mdMap->find(pDepthMarketData->InstrumentID, mdo);
			// Start to trigger pricing
			TriggerUpdateByMarketData(mdo);
		}
	}
}

int CTPOTCWorkerProcessor::ResubMarketData(void)
{
	int ret = 0;

	if (auto mdMap = PricingDataContext()->GetMarketDataMap())
	{
		for (auto it : mdMap->lock_table())
		{
			if (SubscribeMarketData(it.first) == 0)
			{
				ret++;
			}
		}
	}

	return ret;
}