#include "ComplexAlgoirthmManager.h"


IAlgorithmBase_Ptr ComplexAlgoirthmManager::FindModel(const std::string & name) const
{
	IAlgorithmBase_Ptr ret = ModelAlgorithmManager::Instance()->FindModel(name);
	if (!ret)
		ret = PricingAlgorithmManager::Instance()->FindModel(name);

	return ret;
}
