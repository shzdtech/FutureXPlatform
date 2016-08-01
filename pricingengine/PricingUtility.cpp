/***********************************************************************
 * Module:  PricingUtility.cpp
 * Author:  milk
 * Modified: 2015年10月15日 22:54:20
 * Purpose: Implementation of the class PricingUtility
 ***********************************************************************/

#include "PricingUtility.h"
#include "PricingAlgorithmManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       PricingUtility::Pricing(const std::string& alg, const std::vector<void*>& params)
// Purpose:    Implementation of PricingUtility::Pricing()
// Parameters:
// - alg
// - params
// Return:     PricingDO_Ptr
////////////////////////////////////////////////////////////////////////

std::shared_ptr<PricingDO> PricingUtility::Pricing(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	std::shared_ptr<PricingDO> ret;

	auto algorithm =
		PricingAlgorithmManager::Instance()->FindAlgorithm(sdo.Algorithm);

	if (algorithm)
	{
		ret = std::static_pointer_cast<PricingDO>(algorithm->Compute(pInputObject, sdo, priceCtx, params));
	}

	return ret;
}

std::shared_ptr<PricingDO>  PricingUtility::Pricing(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	IPricingDataContext& priceCtx)
{
	return Pricing(pInputObject, sdo, priceCtx, nullptr);
}

std::shared_ptr<PricingDO> PricingUtility::Pricing(
	const void* pInputObject,
	const ContractKey& contractKey,
	IPricingDataContext& priceCtx,
	const param_vector* params)
{
	auto it = priceCtx.GetStrategyMap()->find(contractKey);
	if (it == priceCtx.GetStrategyMap()->end())
		return nullptr;

	return Pricing(pInputObject, it->second, priceCtx);
}

std::shared_ptr<PricingDO> PricingUtility::Pricing(
	const void* pInputObject,
	const ContractKey& contractKey,
	IPricingDataContext& priceCtx)
{
	return Pricing(pInputObject, contractKey, priceCtx, nullptr);
}