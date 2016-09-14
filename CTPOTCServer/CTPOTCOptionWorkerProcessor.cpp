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

void CTPOTCOptionWorkerProcessor::TriggerOTCPricing(const StrategyContractDO& strategyDO)
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


ProductType CTPOTCOptionWorkerProcessor::GetContractProductType() const
{
	return ProductType::PRODUCT_OTC_OPTION;
}

const std::vector<ProductType>& CTPOTCOptionWorkerProcessor::GetStrategyProductTypes() const
{
	static const std::vector<ProductType> productTypes = { ProductType::PRODUCT_OTC_OPTION, ProductType::PRODUCT_OPTIONS };
	return productTypes;
}


InstrumentCache & CTPOTCOptionWorkerProcessor::GetInstrumentCache()
{
	static InstrumentCache& cache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_OTC_OPTION);
	return cache;
}


//CTP APIs
void CTPOTCOptionWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	if (auto pMDO = PricingDataContext()->GetMarketDataMap()->tryfind(pDepthMarketData->InstrumentID))
	{
		if (pMDO->Bid().Price != pDepthMarketData->BidPrice1 ||
			pMDO->Ask().Price != pDepthMarketData->AskPrice1)
		{
			pMDO->Bid().Price = pDepthMarketData->BidPrice1;
			pMDO->Ask().Price = pDepthMarketData->AskPrice1;

			if (_exchangeStrategySet.find(*pMDO) != _exchangeStrategySet.end())
			{
				if (auto pStrategyDO = PricingDataContext()->GetStrategyMap()->tryfind(*pMDO))
				{
					TriggerTadingDeskParams(*pStrategyDO);
					TriggerOTCUpdating(*pStrategyDO);
				}
			}
		}
	}
}
