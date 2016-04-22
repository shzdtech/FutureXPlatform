/***********************************************************************
 * Module:  PricingUtility.cpp
 * Author:  milk
 * Modified: 2015年10月15日 22:54:20
 * Purpose: Implementation of the class PricingUtility
 ***********************************************************************/

#include "PricingUtility.h"
#include "AlgorithmManager.h"
#include "PricingContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       PricingUtility::Pricing(const std::string& alg, const std::vector<void*>& params)
// Purpose:    Implementation of PricingUtility::Pricing()
// Parameters:
// - alg
// - params
// Return:     PricingDO_Ptr
////////////////////////////////////////////////////////////////////////

std::shared_ptr<PricingDO> PricingUtility::Pricing(const StrategyContractDO& strategy, ParamVector& params)
{
	std::shared_ptr<PricingDO> ret;

	auto algorithm =
		AlgorithmManager::Instance()->FindAlgorithm(strategy.Algorithm);

	if (algorithm)
	{
		ret = std::static_pointer_cast<PricingDO>(algorithm->Compute(params));
	}

	return ret;
}


std::shared_ptr<PricingDO> PricingUtility::Pricing(const StrategyContractDO& strategy, const int quantity)
{
	auto pMdMap = PricingContext::GetMarketDataDOMap();
	auto pContractMap = PricingContext::GetContractMap();
	ParamVector params{ &quantity, &strategy, pMdMap, pContractMap };
	return Pricing(strategy, params);
}

std::shared_ptr<PricingDO> PricingUtility::Pricing(const ContractKey& contractKey, const int quantity)
{
	auto pMdMap = PricingContext::GetStrategyMap();
	auto it = pMdMap->find(contractKey);
	if (it == pMdMap->end())
		return nullptr;

	return Pricing(it->second, quantity);
}