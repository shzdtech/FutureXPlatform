/***********************************************************************
 * Module:  ETPPricingAlgorithm.cpp
 * Author:  milk
 * Modified: 2015年9月6日 23:23:15
 * Purpose: Implementation of the class ETPPricingAlgorithm
 ***********************************************************************/

#include "ETPPricingAlgorithm.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractParamDO.h"
#include "../dataobject/TypedefDO.h"


const std::string ETPParams::coeff_name("coeff");
const std::string ETPParams::offset_name("offset");
const std::string ETPParams::spread_name("spread");

 ////////////////////////////////////////////////////////////////////////
 // Name:       ETPPricingAlgorithm::Name()
 // Purpose:    Implementation of ETPPricingAlgorithm::Name()
 // Return:     std::string
 ////////////////////////////////////////////////////////////////////////

const std::string& ETPPricingAlgorithm::Name(void) const
{
	static const std::string name("etp");
	return name;
}

////////////////////////////////////////////////////////////////////////
// Name:       ETPPricingAlgorithm::Compute(std::vector<void*>& params)
// Purpose:    Implementation of ETPPricingAlgorithm::Compute()
// Parameters:
// - params
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

std::shared_ptr<PricingDO> ETPPricingAlgorithm::Compute(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	ETPParams paramObj;
	if (!ParseParams(sdo.ModelParams, &paramObj))
		return nullptr;

	std::shared_ptr<PricingDO> ret;

	auto& mdDOMap = *(priceCtx.GetMarketDataMap());
	auto& conDOMap = *(priceCtx.GetContractParamMap());

	double BidPrice = 0;
	double AskPrice = 0;
	double quantity = *(int*)pInputObject;

	if (sdo.PricingContracts.size() > 0)
	{

		for (auto& conparam : sdo.PricingContracts)
		{
			auto& baseCon = conDOMap.at(conparam);
			auto& md = mdDOMap.at(conparam.InstrumentID());

			double K = std::fabs(conparam.Weight) *
				quantity * sdo.Multiplier / baseCon.Multiplier;


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

		BidPrice = paramObj.coeff * BidPrice + paramObj.offset - paramObj.spread;
		AskPrice = paramObj.coeff * AskPrice + paramObj.offset + paramObj.spread;

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

const std::map<std::string, double>& ETPPricingAlgorithm::DefaultParams(void)
{
	static std::map<std::string, double> defaultParams = {
		{ ETPParams::coeff_name, 1 },
		{ ETPParams::offset_name, 0 },
		{ ETPParams::spread_name, 1 }
	};
	return defaultParams;
}

bool ETPPricingAlgorithm::ParseParams(const ModelParamsDO& modelParams, void * pParamObj)
{
	bool ret = true;

	ETPParams* pParams = (ETPParams*)pParamObj;
	pParams->coeff = modelParams.ScalaParams.at(ETPParams::coeff_name);
	pParams->offset = modelParams.ScalaParams.at(ETPParams::offset_name);
	pParams->spread = modelParams.ScalaParams.at(ETPParams::spread_name);

	return ret;
}
