#if !defined(__pricingengine_ComplexAlgoirthmManager_h)
#define __pricingengine_ComplexAlgoirthmManager_h

#include "ModelAlgorithmManager.h"
#include "PricingAlgorithmManager.h"
#include "../utility/singleton_templ.h"
#include "pricingengine_exp.h"

class PRICINGENGINE_CLASS_EXPORTS ComplexAlgoirthmManager : public singleton_ptr<ComplexAlgoirthmManager>
{
public:
	IAlgorithmBase_Ptr FindModel(const std::string& name) const;
};

#endif;
