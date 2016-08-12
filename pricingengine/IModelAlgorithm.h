#if !defined(__pricingengine_IModelAlgorithm_h)
#define __pricingengine_IModelAlgorithm_h

#include <string>
#include <memory>
#include "IAlgorithmBase.h"
#include "../dataobject/ModelParamsDO.h"
#include "../common/typedefs.h"

class IModelAlgorithm : public IAlgorithmBase
{
public:
	virtual dataobj_ptr Compute(
		const void* pInputObject,
		const ModelParamsDO& modelParams,
		const param_vector* params) = 0;
};

typedef std::shared_ptr<IModelAlgorithm> IModelAlgorithm_Ptr;

#endif