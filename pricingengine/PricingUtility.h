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
	static IPricingDO_Ptr Pricing(
		const void* pInputObject,
		const StrategyContractDO& strategy,
		const IPricingDataContext_Ptr& priceCtx_Ptr,
		const param_vector* params);

	static IPricingDO_Ptr Pricing(
		const void* pInputObject,
		const StrategyContractDO& strategy,
		const IPricingDataContext_Ptr& priceCtx_Ptr);

	static IPricingDO_Ptr Pricing(
		const void* pInputObject,
		const UserContractKey& strategyKey,
		const IPricingDataContext_Ptr& priceCtx_Ptr,
		const param_vector* params);

	static IPricingDO_Ptr Pricing(
		const void* pInputObject,
		const UserContractKey& strategyKey,
		const IPricingDataContext_Ptr& priceCtx_Ptr);


protected:
private:

};

#endif