/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#include "CTPOTCOptionWorkerProcessor.h"
#include "../pricingengine/PricingUtility.h"
#include "../message/DefMessageID.h"
#include "../litelogger/LiteLogger.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of CTPOTCWorkerProcessor::CTPOTCWorkerProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPOTCOptionWorkerProcessor::CTPOTCOptionWorkerProcessor(
	IServerContext* pServerCtx, 
	const std::shared_ptr<CTPOTCTradeProcessor>& otcTradeProcessorPtr) :
	CTPOTCWorkerProcessor(pServerCtx, otcTradeProcessorPtr)
{

}

CTPOTCOptionWorkerProcessor::~CTPOTCOptionWorkerProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

void CTPOTCOptionWorkerProcessor::TriggerPricing(const StrategyContractDO& strategyDO)
{
	if (strategyDO.Enabled)
	{
		if (auto pricingCtx = PricingDataContext())
		{
			auto& mdDO = pricingCtx->GetMarketDataMap()->at(strategyDO.PricingContracts[0].InstrumentID());

			if (auto pricingDO = PricingUtility::Pricing(&mdDO, strategyDO, *pricingCtx))
			{
				_pricingNotifers->foreach(strategyDO, [this, pricingDO](IMessageSession* pSession)
				{ this->SendDataObject(pSession, MSG_ID_RTN_PRICING, 0, pricingDO); }
				);
			}
		}
	}
}

ProductType CTPOTCOptionWorkerProcessor::GetProductType()
{
	return ProductType::PRODUCT_OPTIONS;
}


//CTP APIs

void CTPOTCOptionWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	auto mdMap = PricingDataContext()->GetMarketDataMap();
	auto it = mdMap->find(pDepthMarketData->InstrumentID);
	if (it != mdMap->end())
	{
		auto& mdo = it->second;
		if (mdo.BidPrice != pDepthMarketData->BidPrice1 ||
			mdo.AskPrice != pDepthMarketData->AskPrice1 )
		{
			mdo.BidPrice = pDepthMarketData->BidPrice1;
			mdo.AskPrice = pDepthMarketData->AskPrice1;

			TriggerUpdating(mdo);
		}
	}
}
