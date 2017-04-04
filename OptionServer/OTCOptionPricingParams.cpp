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
		ret = std::make_shared<TradingDeskOptionParams>(pStrategy->ExchangeID(), pStrategy->InstrumentID());

		if (!pStrategy->IsOTC())
		{
			MarketDataDO mDO;
			if (pricingCtx.GetMarketDataMap()->find(pStrategy->InstrumentID(), mDO))
			{
				ret->MarketData = std::make_shared<PricingDO>();
				ret->MarketData->TBid().Price = mDO.Bid().Price;
				ret->MarketData->TBid().Volume = mDO.Bid().Volume;
				ret->MarketData->TAsk().Price = mDO.Ask().Price;
				ret->MarketData->TAsk().Volume = mDO.Ask().Volume;
			}
		}

		// Implied Volatility Model
		if (auto ivmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(pStrategy->IVModel->Model))
		{
			if (auto result = ivmodel_ptr->Compute(nullptr, *pStrategy, pricingCtx, nullptr))
			{
				ret->ImpliedVol = std::make_shared<ImpliedVolatility>(((TDataObject<ImpliedVolatility>*)result.get())->Data);
				auto pOptionParams = (OptionParams*)pStrategy->IVModel->ParsedParams.get();
				if (!std::isnan(ret->ImpliedVol->AskVolatility))
					pOptionParams->askVolatility = ret->ImpliedVol->AskVolatility;
				if (!std::isnan(ret->ImpliedVol->BidVolatility))
					pOptionParams->bidVolatility = ret->ImpliedVol->BidVolatility;
			}
		}

		// Volatility Model
		if (pStrategy->VolModel)
			if (auto volmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(pStrategy->VolModel->Model))
				if (auto result = volmodel_ptr->Compute(nullptr, *pStrategy, pricingCtx, nullptr))
				{
					ret->TheoData = std::static_pointer_cast<WingsModelReturnDO>(result);
					if (auto pOptionParams = (OptionParams*)pStrategy->PricingModel->ParsedParams.get())
					{
						if (!std::isnan(ret->TheoData->TBid().Volatility))
							pOptionParams->bidVolatility = ret->TheoData->TBid().Volatility;
						if (!std::isnan(ret->TheoData->TAsk().Volatility))
							pOptionParams->askVolatility = ret->TheoData->TAsk().Volatility;
					}

					// Temp Volatility Model
					if (auto tempModel_Ptr = StrategyModelCache::FindTempModel(*pStrategy->VolModel))
					{
						StrategyContractDO tempSto(*pStrategy);
						tempSto.VolModel = tempModel_Ptr;
						if (auto tempVol = volmodel_ptr->Compute(nullptr, tempSto, pricingCtx, nullptr))
						{
							ret->TheoDataTemp = std::static_pointer_cast<WingsModelReturnDO>(tempVol);
						}
					}

					// Pricing Model				
					if (IPricingAlgorithm_Ptr prcingmodel_ptr = PricingAlgorithmManager::Instance()->FindModel(pStrategy->PricingModel->Model))
					{
						if (auto result = prcingmodel_ptr->Compute(nullptr, *pStrategy, pricingCtx, nullptr))
						{
							auto pOptionPricing = (OptionPricingDO*)result.get();
							ret->TheoData->TBid().Price = pOptionPricing->TBid().Price;
							ret->TheoData->TBid().Delta = pOptionPricing->TBid().Delta;
							ret->TheoData->TBid().Gamma = pOptionPricing->TBid().Gamma;
							ret->TheoData->TBid().Vega = pOptionPricing->TBid().Vega;
							ret->TheoData->TBid().Theta = pOptionPricing->TBid().Theta;

							ret->TheoData->TAsk().Price = pOptionPricing->TAsk().Price;
							ret->TheoData->TAsk().Delta = pOptionPricing->TAsk().Delta;
							ret->TheoData->TAsk().Gamma = pOptionPricing->TAsk().Gamma;
							ret->TheoData->TAsk().Vega = pOptionPricing->TAsk().Vega;
							ret->TheoData->TAsk().Theta = pOptionPricing->TAsk().Theta;
						}
					}
				}
	}

	return ret;
}
