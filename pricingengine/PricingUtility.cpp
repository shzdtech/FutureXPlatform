/***********************************************************************
 * Module:  PricingUtility.cpp
 * Author:  milk
 * Modified: 2015年10月15日 22:54:20
 * Purpose: Implementation of the class PricingUtility
 ***********************************************************************/

#include "PricingUtility.h"
#include "PricingAlgorithmManager.h"
#include "../message/BizError.h"

////////////////////////////////////////////////////////////////////////
// Name:       PricingUtility::Pricing(const std::string& alg, const std::vector<void*>& params)
// Purpose:    Implementation of PricingUtility::Pricing()
// Parameters:
// - alg
// - params
// Return:     IPricingDO_Ptr
////////////////////////////////////////////////////////////////////////

IPricingDO_Ptr PricingUtility::Pricing(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	const IPricingDataContext_Ptr& priceCtx_Ptr,
	const param_vector* params)
{
	IPricingDO_Ptr ret;

	if (sdo.PricingModel)
	{
		auto algorithm = PricingAlgorithmManager::Instance()->FindModel(sdo.PricingModel->Model);

		if (!algorithm)
			throw NotFoundException("Prcing algorithm '" + sdo.PricingModel->Model + "' not found");

		ret = algorithm->Compute(pInputObject, sdo, priceCtx_Ptr, params);
	}

	return ret;
}

IPricingDO_Ptr PricingUtility::Pricing(
	const void* pInputObject,
	const StrategyContractDO& sdo,
	const IPricingDataContext_Ptr& priceCtx_Ptr)
{
	return Pricing(pInputObject, sdo, priceCtx_Ptr, nullptr);
}

IPricingDO_Ptr PricingUtility::Pricing(
	const void* pInputObject,
	const UserContractKey& strategyKey,
	const IPricingDataContext_Ptr& priceCtx_Ptr,
	const param_vector* params)
{
	StrategyContractDO_Ptr strategy_ptr;
	if (priceCtx_Ptr->GetStrategyMap()->find(strategyKey, strategy_ptr))
		throw NotFoundException("Prcing strategy for " + strategyKey.InstrumentID() + "not found");

	return Pricing(pInputObject, *strategy_ptr, priceCtx_Ptr);
}

IPricingDO_Ptr PricingUtility::Pricing(
	const void* pInputObject,
	const UserContractKey& strategyKey,
	const IPricingDataContext_Ptr& priceCtx_Ptr)
{
	return Pricing(pInputObject, strategyKey, priceCtx_Ptr, nullptr);
}