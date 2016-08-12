/***********************************************************************
 * Module:  BetaSumPricingAlgorithm.cpp
 * Author:  milk
 * Modified: 2015年8月9日 0:42:12
 * Purpose: Implementation of the class BetaSumPricingAlgorithm
 ***********************************************************************/

#include "BetaSumPricingAlgorithm.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractParamDO.h"
#include "../dataobject/TypedefDO.h"

const std::string BetaSumParams::offset_name("offset");
const std::string BetaSumParams::spread_name("spread");

////////////////////////////////////////////////////////////////////////
// Name:       BetaSumPricingAlgorithm::Name()
// Purpose:    Implementation of BetaSumPricingAlgorithm::Name()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& BetaSumPricingAlgorithm::Name(void) const
{
	static const std::string name("bs");
	return name;
}

////////////////////////////////////////////////////////////////////////
// Name:       BetaSumPricingAlgorithm::Compute(std::vector<void*> params)
// Purpose:    Implementation of BetaSumPricingAlgorithm::Compute()
// Parameters:
// - params
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

std::shared_ptr<PricingDO> BetaSumPricingAlgorithm::Compute(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	BetaSumParams paramObj;
	if (!ParseParams(sdo.ModelParams, &paramObj))
		return nullptr;

	std::shared_ptr<PricingDO> ret;

	auto& mdDOMap = *(priceCtx.GetMarketDataMap());
	auto& conDOMap = *(priceCtx.GetContractParamMap());

	double BidPrice = 0;
	double AskPrice = 0;
	int quantity = *(int*)pInputObject;

	if (sdo.PricingContracts.size() > 0)
	{

		for (auto& conparam : sdo.PricingContracts)
		{
			auto& baseCon = conDOMap.at(conparam);
			auto& md = mdDOMap.at(conparam.InstrumentID());

			double K = std::fabs(conparam.Weight) *	quantity * sdo.Multiplier /
				baseCon.Multiplier;


			double VolAdjBidPrice =
				md.BidPrice - baseCon.TickSize *
				(std::fmax(K - md.BidVolume, 0.0) / baseCon.DepthVol + baseCon.Gamma);

			double VolAdjAskPrice =
				md.AskPrice + baseCon.TickSize *
				(std::fmax(K - md.AskVolume, 0.0) / baseCon.DepthVol + baseCon.Gamma);

			if (conparam.Weight < 0)
				std::swap(VolAdjBidPrice, VolAdjAskPrice);

			BidPrice += conparam.Weight * VolAdjBidPrice;

			AskPrice += conparam.Weight * VolAdjAskPrice;
		}

		BidPrice += paramObj.offset - paramObj.spread;
		AskPrice += paramObj.offset + paramObj.spread;

		BidPrice = std::floor(BidPrice / sdo.TickSize) * sdo.TickSize;

		AskPrice = std::ceil(AskPrice / sdo.TickSize) * sdo.TickSize;

		if (!sdo.IsOTC())
		{
			auto& md = mdDOMap.at(sdo.InstrumentID());
			BidPrice = std::fmin(BidPrice, md.BidPrice);
			AskPrice = std::fmax(AskPrice, md.AskPrice);
		}

		PricingDO* pDO = new PricingDO(sdo.ExchangeID(), sdo.InstrumentID());
		ret.reset(pDO);

		pDO->BidPrice = BidPrice;
		pDO->AskPrice = AskPrice;
	}

	return ret;
}

const std::map<std::string, double>& BetaSumPricingAlgorithm::DefaultParams(void)
{
	static std::map<std::string, double> defaultParams = { 
		{ BetaSumParams::offset_name , 0},
		{ BetaSumParams::spread_name , 1},
	};
	return defaultParams;
}

bool BetaSumPricingAlgorithm::ParseParams(const ModelParamsDO& modelParams, void * pParamObj)
{
	bool ret = true;

	BetaSumParams* pParams = (BetaSumParams*)pParamObj;
	pParams->offset = modelParams.ScalaParams.at(BetaSumParams::offset_name);
	pParams->spread = modelParams.ScalaParams.at(BetaSumParams::spread_name);

	return ret;
}
