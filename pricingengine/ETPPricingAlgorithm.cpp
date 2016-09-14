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

IPricingDO_Ptr ETPPricingAlgorithm::Compute(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	if (!sdo.PricingModel && !sdo.PricingModel->ParsedParams)
		return nullptr;

	auto paramObj = (ETPParams*)sdo.VolModel->ParsedParams.get();

	IPricingDO_Ptr ret;

	auto& mdDOMap = *(priceCtx.GetMarketDataMap());
	auto& conDOMap = *(priceCtx.GetContractParamMap());

	double BidPrice = 0;
	double AskPrice = 0;
	double quantity = *(int*)pInputObject;

	if (!sdo.PricingContracts.empty())
	{

		for (auto& conparam : sdo.PricingContracts)
		{
			auto& baseCon = conDOMap.at(conparam);
			auto& md = mdDOMap.at(conparam.InstrumentID());

			double K = std::fabs(conparam.Weight) *
				quantity * sdo.Multiplier / baseCon.Multiplier;


			double VolAdjBidPrice =
				md.Bid().Price - baseCon.TickSize *
				(std::fmax(K - md.Bid().Volume, 0.0) / baseCon.DepthVol + baseCon.Gamma);

			double VolAdjAskPrice =
				md.Ask().Price + baseCon.TickSize *
				(std::fmax(K - md.Ask().Volume, 0.0) / baseCon.DepthVol + baseCon.Gamma);

			if (conparam.Weight < 0)
				std::swap(VolAdjBidPrice, VolAdjAskPrice);

			BidPrice += conparam.Weight * VolAdjBidPrice;

			AskPrice += conparam.Weight * VolAdjAskPrice;
		}

		BidPrice = paramObj->coeff * BidPrice + paramObj->offset - paramObj->spread;
		AskPrice = paramObj->coeff * AskPrice + paramObj->offset + paramObj->spread;

		BidPrice = std::floor(BidPrice / sdo.TickSize) * sdo.TickSize;
		AskPrice = std::ceil(AskPrice / sdo.TickSize) * sdo.TickSize;

		if (!sdo.IsOTC())
		{
			auto& md = mdDOMap.at(sdo.InstrumentID());
			BidPrice = std::fmin(BidPrice, md.Bid().Price);
			AskPrice = std::fmax(AskPrice, md.Ask().Price);
		}

		PricingDO* pDO = new PricingDO(sdo.ExchangeID(), sdo.InstrumentID());
		ret.reset(pDO);

		pDO->Bid().Price = BidPrice;
		pDO->Ask().Price = AskPrice;
	}

	return ret;
}

const std::map<std::string, double>& ETPPricingAlgorithm::DefaultParams(void) const
{
	static std::map<std::string, double> defaultParams = {
		{ ETPParams::coeff_name, 1 },
		{ ETPParams::offset_name, 0 },
		{ ETPParams::spread_name, 0 }
	};
	return defaultParams;
}

std::shared_ptr<void> ETPPricingAlgorithm::ParseParams(const std::map<std::string, double>& modelParams)
{
	auto ret = std::make_shared<ETPParams>();

	ret->coeff = modelParams.at(ETPParams::coeff_name);
	ret->offset = modelParams.at(ETPParams::offset_name);
	ret->spread = modelParams.at(ETPParams::spread_name);

	return ret;
}
