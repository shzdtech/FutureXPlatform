#include "OTCOptionPricingParams.h"
#include "../bizutility/StrategyModelCache.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TradingDeskOptionParams.h"
#include "../pricingengine/ComplexAlgoirthmManager.h"
#include "../pricingengine/OptionParams.h"
#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"

dataobj_ptr OTCOptionPricingParams::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto pStrategy = (StrategyContractDO*)rawRespParams[0];

	std::shared_ptr<TradingDeskOptionParams> ret;

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto& pricingCtx = *pWorkerProc->PricingDataContext();

		MarketDataDO mDO;
		if (!pricingCtx.GetMarketDataMap()->find(pStrategy->PricingContracts[0].InstrumentID(), mDO) ||
			!pricingCtx.GetMarketDataMap()->contains(pStrategy->InstrumentID()))
			return ret;

		ret = std::make_shared<TradingDeskOptionParams>(pStrategy->ExchangeID(), pStrategy->InstrumentID());
		ret->MarketData.TBid().Price = mDO.Bid().Price;
		ret->MarketData.TBid().Volume = mDO.Bid().Volume;
		ret->MarketData.TAsk().Price = mDO.Ask().Price;
		ret->MarketData.TAsk().Volume = mDO.Ask().Volume;

		// Implied Volatility Model
		if (auto ivmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(pStrategy->IVModel->Model))
		{
			if (auto result = ivmodel_ptr->Compute(nullptr, *pStrategy, pricingCtx, nullptr))
			{
				auto& impliedVolBidAsk = ((TDataObject<std::pair<double, double>>*)result.get())->Data;
				ret->MarketData.TBid().Volatility = impliedVolBidAsk.first;
				ret->MarketData.TAsk().Volatility = impliedVolBidAsk.second;
			}
		}

		// Volatility Model
		if (auto volmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(pStrategy->VolModel->Model))
		{
			auto f_atm = (mDO.Ask().Price + mDO.Bid().Price) / 2;
			if (auto result = volmodel_ptr->Compute(&f_atm, *pStrategy, pricingCtx, nullptr))
			{
				auto& volBidAsk = ((TDataObject<std::pair<double, double>>*)result.get())->Data;
				ret->TheoData.TBid().Volatility = volBidAsk.first;
				ret->TheoData.TAsk().Volatility = volBidAsk.second;

				// Temp Volatility Model
				if (auto tempModel_Ptr = StrategyModelCache::FindTempModel(*pStrategy->VolModel))
				{
					StrategyContractDO tempSto(*pStrategy);
					tempSto.VolModel = tempModel_Ptr;
					if (auto tempVol = volmodel_ptr->Compute(&f_atm, tempSto, pricingCtx, nullptr))
					{
						auto& volBidAsk = ((TDataObject<std::pair<double, double>>*)result.get())->Data;
						ret->TheoDataTemp = std::make_shared<OptionPricingDO>();
						ret->TheoDataTemp->TBid().Volatility = volBidAsk.first;
						ret->TheoDataTemp->TAsk().Volatility = volBidAsk.second;
					}
				}

				auto pOptionParams = (OptionParams*)pStrategy->PricingModel->ParsedParams.get();
				pOptionParams->bidVolatility = volBidAsk.first;
				pOptionParams->askVolatility = volBidAsk.second;

				// Pricing Model				
				if (auto prcingmodel_ptr = PricingAlgorithmManager::Instance()->FindModel(pStrategy->PricingModel->Model))
				{
					if (auto result = prcingmodel_ptr->Compute(nullptr, *pStrategy, pricingCtx, nullptr))
					{
						auto pOptionPricing = (OptionPricingDO*)result.get();
						ret->TheoData.TBid().Price = pOptionPricing->TBid().Price;
						ret->TheoData.TBid().Delta = pOptionPricing->TBid().Delta;
						ret->TheoData.TBid().Gamma = pOptionPricing->TBid().Gamma;
						ret->TheoData.TBid().Vega = pOptionPricing->TBid().Vega;
						ret->TheoData.TBid().Theta = pOptionPricing->TBid().Theta;

						ret->TheoData.TAsk().Price = pOptionPricing->TAsk().Price;
						ret->TheoData.TAsk().Delta = pOptionPricing->TAsk().Delta;
						ret->TheoData.TAsk().Gamma = pOptionPricing->TAsk().Gamma;
						ret->TheoData.TAsk().Vega = pOptionPricing->TAsk().Vega;
						ret->TheoData.TAsk().Theta = pOptionPricing->TAsk().Theta;
					}
				}
			}
		}
	}

	return ret;
}
