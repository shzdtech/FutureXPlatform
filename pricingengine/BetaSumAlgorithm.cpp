/***********************************************************************
 * Module:  BetaSumAlgorithm.cpp
 * Author:  milk
 * Modified: 2015年8月9日 0:42:12
 * Purpose: Implementation of the class BetaSumAlgorithm
 ***********************************************************************/

#include "BetaSumAlgorithm.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParam.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractDO.h"
#include "../dataobject/TypedefDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       BetaSumAlgorithm::Name()
// Purpose:    Implementation of BetaSumAlgorithm::Name()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& BetaSumAlgorithm::Name(void) const
{
	static const std::string name("bs");
	return name;
}

////////////////////////////////////////////////////////////////////////
// Name:       BetaSumAlgorithm::Compute(std::vector<void*> params)
// Purpose:    Implementation of BetaSumAlgorithm::Compute()
// Parameters:
// - params
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr BetaSumAlgorithm::Compute(
	const StrategyContractDO& sdo,
	double inputVal,
	PricingContext& priceCtx,
	const ParamVector* params) const
{
	static const std::string alpha_name("alpha");

	dataobj_ptr ret;

	auto& mdDOMap = *(priceCtx.GetMarketDataDOMap());
	auto& conDOMap = *(priceCtx.GetContractMap());

	auto& parentCon = conDOMap.at(sdo);

	double bias = sdo.ParamMap->at(alpha_name) + sdo.Offset;
	double BidPrice = 0;
	double AskPrice = 0;
	double quantity = inputVal;

	if (sdo.BaseContracts && sdo.BaseContracts->size() > 0)
	{

		for (auto& conparam : *(sdo.BaseContracts))
		{
			auto& baseCon = conDOMap.at(conparam);
			auto& md = mdDOMap.at(conparam.InstrumentID());

			double K = std::fabs(conparam.Weight) *	quantity * parentCon.Multiplier / 
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

		BidPrice += bias - sdo.Spread;
		AskPrice += bias + sdo.Spread;

		BidPrice = std::floor(BidPrice / parentCon.TickSize) * parentCon.TickSize;

		AskPrice = std::ceil(AskPrice / parentCon.TickSize) * parentCon.TickSize;

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