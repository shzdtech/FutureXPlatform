#include "OTCOptionPricingParams.h"
#include "../bizutility/StrategyModelCache.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TradingDeskOptionParams.h"
#include "../pricingengine/ComplexAlgoirthmManager.h"
#include "../pricingengine/OptionParams.h"
#include "../pricingengine/PricingUtility.h"
#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"

dataobj_ptr OTCOptionPricingParams::GenerateTradingDeskData(const StrategyContractDO& sto, const IPricingDataContext_Ptr& pricingCtx, bool triggerPricing)
{
	std::shared_ptr<TradingDeskOptionParams> ret;

	if (pricingCtx)
	{
		ret = std::make_shared<TradingDeskOptionParams>(sto.ExchangeID(), sto.InstrumentID());

		if (!sto.IsOTC())
		{
			MarketDataDO mDO;
			if (pricingCtx->GetMarketDataMap()->find(sto.InstrumentID(), mDO))
			{
				ret->MarketData = std::make_shared<PricingDO>(mDO);
			}
		}

		if (triggerPricing)
		{
			// Implied Volatility Model
			if (sto.IVModel)
				if (auto ivmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(sto.IVModel->Model))
				{
					if (auto result = ivmodel_ptr->Compute(nullptr, sto, pricingCtx, nullptr))
					{
						ret->ImpliedVol = std::make_shared<ImpliedVolatility>(((TDataObject<ImpliedVolatility>*)result.get())->Data);
						auto pOptionParams = (OptionParams*)sto.IVModel->ParsedParams.get();
						if (!std::isnan(ret->ImpliedVol->AskVolatility))
							pOptionParams->askVolatility = ret->ImpliedVol->AskVolatility;
						if (!std::isnan(ret->ImpliedVol->BidVolatility))
							pOptionParams->bidVolatility = ret->ImpliedVol->BidVolatility;
					}
				}

			// Volatility Model
			if (sto.VolModel)
				if (auto volmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(sto.VolModel->Model))
					if (auto result = volmodel_ptr->Compute(nullptr, sto, pricingCtx, nullptr))
					{
						ret->TheoData = std::static_pointer_cast<WingsModelReturnDO>(result);
						if (auto pOptionParams = (OptionParams*)sto.PricingModel->ParsedParams.get())
						{
							if (!std::isnan(ret->TheoData->Volatility))
								pOptionParams->midVolatility = ret->TheoData->Volatility;
							if (!std::isnan(ret->TheoData->TBid().Volatility))
								pOptionParams->bidVolatility = ret->TheoData->TBid().Volatility;
							if (!std::isnan(ret->TheoData->TAsk().Volatility))
								pOptionParams->askVolatility = ret->TheoData->TAsk().Volatility;
						}

						// Temp Volatility Model
						if (auto tempModel_Ptr = StrategyModelCache::FindTempModel(*sto.VolModel))
						{
							StrategyContractDO tempSto(sto);
							tempSto.VolModel = tempModel_Ptr;
							if (auto tempVol = volmodel_ptr->Compute(nullptr, tempSto, pricingCtx, nullptr))
							{
								ret->TheoDataTemp = std::static_pointer_cast<WingsModelReturnDO>(tempVol);
							}
						}
					}

			// Pricing Model	
			if (auto pricingDO = PricingUtility::Pricing(nullptr, sto, pricingCtx))
			{
				if (!ret->TheoData)
					ret->TheoData = std::make_shared<WingsModelReturnDO>();

				ret->TheoData->Bid().Price = pricingDO->Bid().Price;
				ret->TheoData->Ask().Price = pricingDO->Ask().Price;

				ret->TheoData->Delta = pricingDO->Delta;
				ret->TheoData->Gamma = pricingDO->Gamma;
				ret->TheoData->Vega = pricingDO->Vega;
				ret->TheoData->Theta = pricingDO->Theta;
				ret->TheoData->Rho = pricingDO->Rho;

				pricingCtx->GetPricingDataDOMap()->upsert(sto, [&pricingDO](IPricingDO_Ptr& pricing_ptr) { pricing_ptr = pricingDO; }, pricingDO);
			}
		}
	}

	return ret;
}
