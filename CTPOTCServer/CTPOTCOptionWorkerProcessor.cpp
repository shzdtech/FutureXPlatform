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
	auto pMarketDataMap = PricingDataContext()->GetMarketDataMap();
	MarketDataDO mDO;
	if(pMarketDataMap->find(pDepthMarketData->InstrumentID, mDO))
	{
		double bidPrice = pDepthMarketData->BidPrice1;
		double askPrice = pDepthMarketData->AskPrice1;
		if (bidPrice < 1e-2 || bidPrice > 1e32 || askPrice < 1e-2 || askPrice > 1e32)
		{
			return;
		}
		if (mDO.Bid().Price != bidPrice || mDO.Ask().Price != askPrice)
		{
			mDO.Bid().Price = bidPrice;
			mDO.Ask().Price = askPrice;
			mDO.Bid().Volume = pDepthMarketData->BidVolume1;
			mDO.Ask().Volume = pDepthMarketData->AskVolume1;

			pMarketDataMap->update(pDepthMarketData->InstrumentID, mDO);

			TriggerUpdating(mDO);
		}
	}
}
