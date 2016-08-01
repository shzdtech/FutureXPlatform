/***********************************************************************
 * Module:  BetaSumPricingAlgorithm.h
 * Author:  milk
 * Modified: 2015年8月9日 0:42:12
 * Purpose: Declaration of the class BetaSumPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingengine_BetaSumPricingAlgorithm_h)
#define __pricingengine_BetaSumPricingAlgorithm_h

#include "IPricingAlgorithm.h"

class BetaSumPricingAlgorithm : public IPricingAlgorithm
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