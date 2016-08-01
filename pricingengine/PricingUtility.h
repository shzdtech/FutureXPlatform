/***********************************************************************
 * Module:  PricingUtility.h
 * Author:  milk
 * Modified: 2015年10月15日 22:54:20
 * Purpose: Declaration of the class PricingUtility
 ***********************************************************************/

#if !defined(__pricingengine_PricingUtility_h)
#define __pricingengine_PricingUtility_h

#include "../dataobject/PricingDO.h"
#include "../common/typedefs.h"
#include "../dataobject/StrategyContractDO.h"
#include "../utility/singleton_templ.h"
#include "IPricingDataContext.h"

#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS PricingUtility
{
public:
	static std::shared_ptr<PricingDO> Pricing(
		const void* pInputObject,
		const StrategyContractDO& strategy,
		IPricingDataContext& priceCtx,
		const param_vector* params);

	static std::shared_ptr<PricingDO> Pricing(
		const void* pInputObject,
		const StrategyContractDO& strategy,
		IPricingDataContext& priceCtx);

	static std::shared_ptr<PricingDO> Pricing(
		const void* pInputObject,
		const ContractKey& contractKey,
		IPricingDataContext& priceCtx,
		const param_vector* params);

	static std::shared_ptr<PricingDO> Pricing(
		const void* pInputObject,
		const ContractKey& contractKey,
		IPricingDataContext& priceCtx);


protected:
private:

};

#endif