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
	const IPricingDataContext_Ptr& priceCtx_Ptr,
	const param_vector* params)
{
	if (!sdo.PricingModel->ParsedParams)
	{
		ParseParams(sdo.PricingModel->Params, sdo.PricingModel->ParsedParams);
	}

	auto paramObj = (ETPParams*)sdo.PricingModel->ParsedParams.get();

	IPricingDO_Ptr ret;

	auto& mdDOMap = *(priceCtx_Ptr->GetMarketDataMap());
	auto& conDOMap = *(priceCtx_Ptr->GetContractParamMap());

	double BidPrice = 0;
	double AskPrice = 0;

	int quantity = pInputObject ? *(int*)pInputObject : sdo.Quantity;

	if (sdo.PricingContracts)
	{
		MarketDataDO md;
		for (auto& conparam : sdo.PricingContracts->PricingContracts)
		{
			auto pBaseCon = conDOMap.tryfind(conparam);
			if (!pBaseCon)
				return ret;

			mdDOMap.find(conparam.InstrumentID(), md);

			double K = std::fabs(conparam.Weight) *	quantity * sdo.Multiplier / pBaseCon->Multiplier;

			double mdBidPrice = md.Bid().Price + conparam.Adjust;
			double mdAskPrice = md.Ask().Price + conparam.Adjust;

			double VolAdjBidPrice =
				mdBidPrice - pBaseCon->TickSize * (std::fmax(K - md.Bid().Volume, 0.0) / pBaseCon->DepthVol + pBaseCon->Gamma);

			double VolAdjAskPrice =
				mdAskPrice + pBaseCon->TickSize * (std::fmax(K - md.Ask().Volume, 0.0) / pBaseCon->DepthVol + pBaseCon->Gamma);

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
			mdDOMap.find(sdo.InstrumentID(), md);
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

void ETPPricingAlgorithm::ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target)
{
	if (!target)
		target = std::make_unique<ETPParams>();

	auto pParam = (ETPParams*)target.get();
	pParam->coeff = modelParams.at(ETPParams::coeff_name);
	pParam->offset = modelParams.at(ETPParams::offset_name);
	pParam->spread = modelParams.at(ETPParams::spread_name);
}
