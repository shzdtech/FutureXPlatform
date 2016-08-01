#if !defined(__pricingengine_IModelAlgorithm_h)
#define __pricingengine_IModelAlgorithm_h

#include <string>
#include <memory>
#include "../dataobject/ModelParamsDO.h"

class IModelAlgorithm
{
public:
	virtual const std::string& Name(void) const = 0;
	virtual dataobj_ptr Compute(const dataobj_ptr& input) = 0;
};

typedef std::shared_ptr<IModelAlgorithm> IModelAlgorithm_Ptr;

#endif