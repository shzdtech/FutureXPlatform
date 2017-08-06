/***********************************************************************
 * Module:  CTPOTCWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年9月2日 14:47:00
 * Purpose: Implementation of the class CTPOTCWorkerProcessor
 ***********************************************************************/

#include "CTPOTCOptionWorkerProcessor.h"
#include "../dataobject/TradingDeskOptionParams.h"
#include "../pricingengine/PricingUtility.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../litelogger/LiteLogger.h"
#include "../OptionServer/OTCOptionPricingParams.h"

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

bool CTPOTCOptionWorkerProcessor::TriggerTradingDeskParams(const StrategyContractDO & strategyDO)
{
	auto pricingCtx = PricingDataContext();

	auto tradingDeskData = OTCOptionPricingParams::GenerateTradingDeskData(strategyDO, pricingCtx, true);

	_tradingDeskNotifers->foreach(strategyDO, [this, &tradingDeskData](const IMessageSession_Ptr& session_ptr)
	{
		SendDataObject(session_ptr, MSG_ID_RTN_TRADINGDESK_PRICING, 0, tradingDeskData);
	});

	return true;
}



//CTP APIs
void CTPOTCOptionWorkerProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	if (!_closing)
	{
		auto pMarketDataMap = PricingDataContext()->GetMarketDataMap();
		bool updated = false;
		bool found = 
			pMarketDataMap->update_fn(pDepthMarketData->InstrumentID, [pDepthMarketData, &updated](MarketDataDO& mDO)
		{
			double bidPrice = pDepthMarketData->BidPrice1;
			double askPrice = pDepthMarketData->AskPrice1;
			if (bidPrice > 1e32)
			{
				bidPrice = pDepthMarketData->LowerLimitPrice;
				if (bidPrice > 1e32)
					bidPrice = pDepthMarketData->PreSettlementPrice;
			}
			if (askPrice > 1e32)
			{
				askPrice = pDepthMarketData->UpperLimitPrice;
				if (askPrice > 1e32)
					askPrice = pDepthMarketData->PreSettlementPrice;
			}

			mDO.SettlementPrice = pDepthMarketData->SettlementPrice < 1e32 ? pDepthMarketData->SettlementPrice : 0;
			mDO.Bid().Volume = pDepthMarketData->BidVolume1;
			mDO.Ask().Volume = pDepthMarketData->AskVolume1;

			if (mDO.Bid().Price != bidPrice || mDO.Ask().Price != askPrice)
			{
				mDO.Bid().Price = bidPrice;
				mDO.Ask().Price = askPrice;
				mDO.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
				mDO.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;

				updated = true;
			}
		});

		if (found)
		{
			MarketDataDO mDO;
			pMarketDataMap->find(pDepthMarketData->InstrumentID, mDO);
			if (updated)
			{
				TriggerUpdateByMarketData(mDO);
			}
		}
	}
}
