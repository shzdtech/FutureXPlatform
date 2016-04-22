/***********************************************************************
 * Module:  ETPAlgorithm.cpp
 * Author:  milk
 * Modified: 2015年9月6日 23:23:15
 * Purpose: Implementation of the class ETPAlgorithm
 ***********************************************************************/

#include "ETPAlgorithm.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParam.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractDO.h"
#include "../dataobject/TypedefDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       ETPAlgorithm::Name()
// Purpose:    Implementation of ETPAlgorithm::Name()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& ETPAlgorithm::Name(void) const
{
	static const std::string name("etp");
	return name;
}

////////////////////////////////////////////////////////////////////////
// Name:       ETPAlgorithm::Compute(std::vector<void*>& params)
// Purpose:    Implementation of ETPAlgorithm::Compute()
// Parameters:
// - params
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr ETPAlgorithm::Compute(ParamVector& params)
{
	static const std::string const1_name("const1");
	static const std::string const2_name("const2");

	dataobj_ptr ret;

	int quantity = *((int*)(params[0]));
	auto& sdo = *((StrategyContractDO*)params[1]);
	auto& mdDOMap = *((MarketDataDOMap*)params[2]);
	auto& conDOMap = *((ContractDOMap*)params[3]);

	auto& parentCon = conDOMap.at(sdo);

	double const1 = sdo.ParamMap->at(const1_name);

	double bias = sdo.ParamMap->at(const2_name) + sdo.Offset;
	double BidPrice = 0;
	double AskPrice = 0;

	if (sdo.BaseContracts && sdo.BaseContracts->size() > 0)
	{

		for (auto& conparam : *(sdo.BaseContracts))
		{
			auto& baseCon = conDOMap.at(conparam);
			auto& md = mdDOMap.at(conparam.InstrumentID());

			double K = std::fabs(conparam.Weight) *
				quantity * parentCon.Multiplier / baseCon.Multiplier;


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

		BidPrice = const1*BidPrice + bias - sdo.Spread;
		AskPrice = const1*AskPrice + bias + sdo.Spread;

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