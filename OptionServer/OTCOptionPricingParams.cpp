#include "OTCOptionPricingParams.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TradingDeskOptionParams.h"
#include "../pricingengine/ComplexAlgoirthmManager.h"
#include "../pricingengine/OptionParams.h"
#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"

dataobj_ptr OTCOptionPricingParams::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto pStrategy = (StrategyContractDO*)rawRespParams[0];

	auto ret = std::make_shared<TradingDeskOptionParams>(pStrategy->ExchangeID(), pStrategy->InstrumentID());

	if (auto wkProcPtr = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(session->getProcessor()))
	{
		auto& pricingCtx = *wkProcPtr->PricingDataContext();

		auto pBaseMD = pricingCtx.GetMarketDataMap()->tryfind(pStrategy->PricingContracts[0].InstrumentID());
		auto pMD = pricingCtx.GetMarketDataMap()->tryfind(pStrategy->InstrumentID());

		if (auto ivmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(pStrategy->IVModel->Model))
		{
			if (auto result = ivmodel_ptr->Compute(nullptr, *pStrategy, pricingCtx, nullptr))
			{
				auto& impliedVolBidAsk = ((TDataObject<std::pair<double, double>>*)result.get())->Data;
				ret->MarketData.TBid().Price = pMD->Bid().Price;
				ret->MarketData.TBid().Volume = pMD->Bid().Volume;
				ret->MarketData.TBid().Volatility = impliedVolBidAsk.first;

				ret->MarketData.TAsk().Price = pMD->Ask().Price;
				ret->MarketData.TAsk().Volume = pMD->Ask().Volume;
				ret->MarketData.TAsk().Volatility = impliedVolBidAsk.second;
			}
		}

		if (auto volmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(pStrategy->VolModel->Model))
		{
			auto f_atm = (pMD->Ask().Price + pMD->Bid().Price) / 2;
			if (auto result = volmodel_ptr->Compute(&f_atm, *pStrategy, pricingCtx, nullptr))
			{
				auto& volBidAsk = ((TDataObject<std::pair<double, double>>*)result.get())->Data;
				ret->TheoData.TBid().Volatility = volBidAsk.first;
				ret->TheoData.TAsk().Volatility = volBidAsk.second;

				auto pOptionParams = (OptionParams*)pStrategy->PricingModel->ParsedParams.get();
				pOptionParams->bidVolatility = volBidAsk.first;
				pOptionParams->askVolatility = volBidAsk.second;
			}
		}

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

	return ret;
}
