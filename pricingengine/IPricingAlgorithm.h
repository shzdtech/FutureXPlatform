/***********************************************************************
 * Module:  IPricingAlgorithm.h
 * Author:  milk
 * Modified: 2015年8月9日 0:49:17
 * Purpose: Declaration of the class IPricingAlgorithm
 ***********************************************************************/

#if !defined(__pricingengine_IPricingAlgorithm_h)
#define __pricingengine_IPricingAlgorithm_h

#include <string>
#include "../dataobject/StrategyContractDO.h"
#include "../common/typedefs.h"
#include "IPricingDataContext.h"

class IPricingAlgorithm
{
public:
	virtual const std::string& Name(void) const = 0;
	virtual dataobj_ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx,
		const param_vector* params) = 0;

protected:
private:

};

typedef std::shared_ptr<IPricingAlgorithm> IPricingAlgorithm_Ptr;

#endif