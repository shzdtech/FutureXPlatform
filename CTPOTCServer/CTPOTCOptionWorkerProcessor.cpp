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
	if (strategyDO.BidEnabled || strategyDO.AskEnabled)
	{
		if (auto pricingCtx = PricingDataContext())
		{
			if (auto pMdDO = pricingCtx->GetMarketDataMap()->tryfind(strategyDO.PricingContracts[0].InstrumentID()))
			{
				if (auto pricingDO = PricingUtility::Pricing(pMdDO, strategyDO, *pricingCtx))
				{
					_pricingNotifers->foreach(strategyDO, [this, pricingDO](const IMessageSession_Ptr& session_ptr)
					{ SendDataObject(session_ptr, MSG_ID_RTN_PRICING, 0, pricingDO); }
					);
				}
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
		double bidPrice = pDepthMarketData->BidPrice1;
		double askPrice = pDepthMarketData->AskPrice1;
		if (bidPrice < 1e-2 || bidPrice > 1e32 || askPrice < 1e-2 || askPrice > 1e32)
		{
			return;
		}
		if (pMDO->Bid().Price != bidPrice || pMDO->Ask().Price != askPrice)
		{
			pMDO->Bid().Price = bidPrice;
			pMDO->Ask().Price = askPrice;
			pMDO->Bid().Volume = pDepthMarketData->BidVolume1;
			pMDO->Ask().Volume = pDepthMarketData->AskVolume1;

			if (_exchangeStrategySet.contains<ContractKey>(*pMDO))
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
