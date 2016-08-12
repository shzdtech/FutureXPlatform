/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#include "CTPOTCOptionWorkerProcessor.h"
#include "../pricingengine/PricingUtility.h"
#include "../message/DefMessageID.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCWorkerProcessor::CTPOTCWorkerProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of CTPOTCWorkerProcessor::CTPOTCWorkerProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPOTCOptionWorkerProcessor::CTPOTCOptionWorkerProcessor(const std::map<std::string, std::string>& configMap,
	IServerContext* pServerCtx, const std::shared_ptr<CTPOTCTradeProcessor>& otcTradeProcessorPtr) :
	CTPOTCWorkerProcessor(configMap, pServerCtx, otcTradeProcessorPtr)
{

}

void CTPOTCOptionWorkerProcessor::TriggerPricing(const StrategyContractDO& strategyDO)
{
	if (strategyDO.Enabled)
	{
		if (auto pricingCtx = GetOTCTradeProcessor()->GetPricingContext())
		{
			auto& mdDO = pricingCtx->GetMarketDataMap()->at(strategyDO.PricingContracts[0].InstrumentID());

			auto pricingDO = PricingUtility::Pricing(&mdDO, strategyDO, *pricingCtx);

			_pricingNotifers->foreach(strategyDO, [this, pricingDO](IMessageSession* pSession)
			{ this->SendDataObject(pSession, MSG_ID_RTN_PRICING, 0, pricingDO); }
			);
		}
	}
}

ProductType CTPOTCOptionWorkerProcessor::GetProductType()
{
	return ProductType::PRODUCT_OPTIONS;
}


