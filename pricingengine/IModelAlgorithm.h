#if !defined(__pricingengine_IModelAlgorithm_h)
#define __pricingengine_IModelAlgorithm_h

#include <string>
#include <memory>
#include "IAlgorithmBase.h"
#include "../dataobject/StrategyContractDO.h"
#include "../common/typedefs.h"
#include "IPricingDataContext.h"

class IModelAlgorithm : public IAlgorithmBase
{
public:
	virtual dataobj_ptr Compute(
		const void* pInputObject,
		const StrategyContractDO& sdo,
		IPricingDataContext& priceCtx,
		const param_vector* params) = 0;
};

typedef std::shared_ptr<IModelAlgorithm> IModelAlgorithm_Ptr;

#endif