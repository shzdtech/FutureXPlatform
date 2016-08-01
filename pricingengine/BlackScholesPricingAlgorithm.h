/***********************************************************************
 * Module:  BlackScholesPricingAlgorithm.h
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Declaration of the class BlackScholesPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingalgorithm_BlackScholesPricingAlgorithm_h)
#define __pricingalgorithm_BlackScholesPricingAlgorithm_h

#include "../pricingengine/IPricingAlgorithm.h"

class BlackScholesPricingAlgorithm : public IPricingAlgorithm
{
public:
   const std::string& Name(void) const;
   dataobj_ptr Compute(
	   const void* pInputObject,
	   const StrategyContractDO& sdo,
	   IPricingDataContext& priceCtx,
	   const param_vector* params);

protected:
	double ComputeOptionPrice(
		double inputPrice,
		int optionType,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx
		);

private:

};

#endif