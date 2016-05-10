/***********************************************************************
 * Module:  BetaSumAlgorithm.h
 * Author:  milk
 * Modified: 2015年8月9日 0:42:12
 * Purpose: Declaration of the class BetaSumAlgorithm
 ***********************************************************************/

#if !defined(__strategy_BetaSumAlgorithm_h)
#define __strategy_BetaSumAlgorithm_h

#include "IAlgorithm.h"

class BetaSumAlgorithm : public IAlgorithm
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