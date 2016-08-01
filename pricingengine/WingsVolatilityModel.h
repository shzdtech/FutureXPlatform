#if !defined(__pricingengine_WingsVolatilityModel_h)
#define __pricingengine_WingsVolatilityModel_h

#include "IModelAlgorithm.h"

class WingsVolatilityModel : public IModelAlgorithm
{
public:
	virtual const std::string& Name(void) const;
	virtual dataobj_ptr Compute(const dataobj_ptr& input);
};

#endif

