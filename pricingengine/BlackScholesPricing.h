/***********************************************************************
 * Module:  BlackScholesPricing.h
 * Author:  milk
 * Modified: 2016年5月9日 22:57:32
 * Purpose: Declaration of the class BlackScholesPricing
 ***********************************************************************/

#if !defined(__pricingalgorithm_BlackScholesPricing_h)
#define __pricingalgorithm_BlackScholesPricing_h

#include "../pricingengine/IAlgorithm.h"

class BlackScholesPricing : public IAlgorithm
{
public:
   const std::string& Name(void) const;
   dataobj_ptr Compute(
	   const StrategyContractDO& sdo,
	   double inputVal,
	   PricingContext& priceCtx,
	   const ParamVector* params) const;

protected:
private:

};

#endif