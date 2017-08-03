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
#include "IAlgorithmBase.h"


class IPricingAlgorithm : public IAlgorithmBase
{
public:
	virtual IPricingDO_Ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		const IPricingDataContext_Ptr& priceCtx_Ptr,
		const param_vector* params) = 0;

protected:
private:

};

typedef std::shared_ptr<IPricingAlgorithm> IPricingAlgorithm_Ptr;

#endif