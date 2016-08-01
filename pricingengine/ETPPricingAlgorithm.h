/***********************************************************************
 * Module:  ETPPricingAlgorithm.h
 * Author:  milk
 * Modified: 2015年9月6日 23:23:15
 * Purpose: Declaration of the class ETPPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingengine_ETPPricingAlgorithm_h)
#define __pricingengine_ETPPricingAlgorithm_h

#include "IPricingAlgorithm.h"

class ETPPricingAlgorithm : public IPricingAlgorithm
{
public:
	const std::string& Name(void) const;
	dataobj_ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx,
		const param_vector* params);

protected:
private:

};

#endif