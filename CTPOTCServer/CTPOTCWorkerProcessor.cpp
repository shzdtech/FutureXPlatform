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
	const std::shared_ptr<CTPOTCTradeWorkerProcessor>& otcTradeProcessorPtr) :
	CTPMarketDataSAProcessor(pServerCtx),
	OTCWorkerProcessor(otcTradeProcessorPtr->PricingDataContext()),
	_CTPOTCTradeWorkerProcessorPtr(otcTradeProcessorPtr)
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
	((CTPOTCTradeWorkerProcessor*)GetOTCTradeWorkerProcessor())->setMessageSession(msgSession_ptr);
}

void CTPOTCWorkerProcessor::setServiceLocator(const IMessageServiceLocator_Ptr& svcLct_Ptr)
{
	CTPMarketDataSAProcessor::setServiceLocator(svcLct_Ptr);
	((CTPOTCTradeWorkerProcessor*)GetOTCTradeWorkerProcessor())->setServiceLocator(svcLct_Ptr);
}

bool CTPOTCWorkerProcessor::OnSessionClosing(void)
{
	_closing = true;

	if (_initializer.valid())
		_initializer.wait();

	GetOTCTradeWorkerProcessor()->Dispose();

	return true;
}


void CTPOTCWorkerProcessor::Initialize(IServerContext* serverCtx)
{
	std::string productTypes;
	if (getServerContext()->getConfigVal("strategy_producttype", productTypes))
	{
		std::vector<std::string> productVec;
		stringutility::split(productTypes, productVec);

		for (auto& strProductType : productVec)
			_strategyProductTypes.emplace((ProductType)std::stoi(strProductType));
	}
	else
	{
		_strategyProductTypes.emplace(ProductType::PRODUCT_OTC);
	}

	OTCWorkerProcessor::Initialize();

	CTPMarketDataSAProcessor::Initialize(serverCtx);
}


int CTPOTCWorkerProcessor::SubscribeMarketData(const ContractKey& contractId)
{
	int ret = -1;
	if (_isLogged && _subedContracts.find(contractId) == _subedContracts.end())
	{
		char* contract[] = { const_cast<char*>(contractId.InstrumentID().data()) };
		if (auto mdApiProxy = _rawAPI->MdAPIProxy())
		{
			ret = mdApiProxy->get()->SubscribeMarketData(contract, 1);
		}

		if (ret == 0)
		{
			_subedContracts.emplace(contractId);
		}
	}

	return ret;
}

void CTPOTCWorkerProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	auto& userInfo = sessionPtr->getUserInfo();
	if (!userInfo.getTradingDay())
	{
		userInfo.setTradingDay(_systemUser.getTradingDay());
	}
	_userSessionCtn_Ptr->add(userInfo.getUserId(), sessionPtr);
}

ProductType CTPOTCWorkerProcessor::GetContractProductType() const
{
	return ProductType::PRODUCT_OTC;
}


OTCTradeWorkerProcessor * CTPOTCWorkerProcessor::GetOTCTradeWorkerProcessor()
{
	return _CTPOTCTradeWorkerProcessorPtr.get();
}

CTPOTCTradeWorkerProcessor * CTPOTCWorkerProcessor::GetCTPOTCTradeWorkerProcessor()
{
	return _CTPOTCTradeWorkerProcessorPtr.get();
}

int CTPOTCWorkerProcessor::ResubMarketData()
{
	for (auto contract : _subedContracts)
	{
		SubscribeMarketData(contract);
	}

	return 0;
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