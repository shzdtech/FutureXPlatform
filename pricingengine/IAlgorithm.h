/***********************************************************************
 * Module:  IAlgorithm.h
 * Author:  milk
 * Modified: 2015年8月9日 0:49:17
 * Purpose: Declaration of the class IAlgorithm
 ***********************************************************************/

#if !defined(__strategy_IAlgorithm_h)
#define __strategy_IAlgorithm_h

#include <string>
#include "../dataobject/StrategyContractDO.h"
#include "../common/typedefs.h"
#include "IPricingDataContext.h"

class IAlgorithm
{
public:
	virtual const std::string& Name(void) const = 0;
	virtual dataobj_ptr Compute(
		const StrategyContractDO& sdo,
		double inputVal,
		IPricingDataContext& priceCtx,
		const param_vector* params) const = 0;

protected:
private:

};

typedef std::shared_ptr<IAlgorithm> IAlgorithm_Ptr;

#endif