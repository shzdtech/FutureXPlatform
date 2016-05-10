/***********************************************************************
 * Module:  PricingUtility.cpp
 * Author:  milk
 * Modified: 2015年10月15日 22:54:20
 * Purpose: Implementation of the class PricingUtility
 ***********************************************************************/

#include "PricingUtility.h"
#include "AlgorithmManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       PricingUtility::Pricing(const std::string& alg, const std::vector<void*>& params)
// Purpose:    Implementation of PricingUtility::Pricing()
// Parameters:
// - alg
// - params
// Return:     PricingDO_Ptr
////////////////////////////////////////////////////////////////////////

std::shared_ptr<PricingDO> PricingUtility::Pricing(
	const StrategyContractDO& strategy,
	double inputVal,
	PricingContext* priceCtx,
	const ParamVector* params)
{
	std::shared_ptr<PricingDO> ret;

	auto algorithm =
		AlgorithmManager::Instance()->FindAlgorithm(strategy.Algorithm);

	if (algorithm)
	{
		ret = std::static_pointer_cast<PricingDO>(algorithm->Compute(strategy, inputVal, *priceCtx, params));
	}

	return ret;
}

std::shared_ptr<PricingDO>  PricingUtility::Pricing(
	const StrategyContractDO& strategy,
	double inputVal,
	PricingContext* priceCtx)
{
	return Pricing(strategy, inputVal, priceCtx, nullptr);
}

std::shared_ptr<PricingDO> PricingUtility::Pricing(
	const ContractKey& contractKey,
	double inputVal,
	PricingContext* priceCtx,
	const ParamVector* params)
{
	auto it = priceCtx->GetStrategyMap()->find(contractKey);
	if (it == priceCtx->GetStrategyMap()->end())
		return nullptr;

	return Pricing(it->second, inputVal);
}

std::shared_ptr<PricingDO> PricingUtility::Pricing(
	const ContractKey& contractKey,
	double inputVal,
	PricingContext* priceCtx)
{
	return Pricing(contractKey, inputVal, priceCtx, nullptr);
}

std::shared_ptr<PricingDO> PricingUtility::Pricing(
	const StrategyContractDO& strategy,
	double inputVal,
	const ParamVector* params)
{
	return Pricing(strategy, inputVal, PricingContext::Instance(), params);
}