#if !defined(__pricingengine_IModelAlgorithm_h)
#define __pricingengine_IModelAlgorithm_h

#include <string>
#include <memory>
#include "IAlgorithmBase.h"
#include "../dataobject/ModelParamsDO.h"

class IModelAlgorithm : public IAlgorithmBase
{
public:
	virtual dataobj_ptr Compute(const dataobj_ptr& input) = 0;
};

typedef std::shared_ptr<IModelAlgorithm> IModelAlgorithm_Ptr;

#endif