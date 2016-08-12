#if !defined(__pricingengine_IAlgorithmBase_h)
#define __pricingengine_IAlgorithmBase_h

#include <string>
#include <map>
#include "../dataobject/ModelParamsDO.h"

class IAlgorithmBase
{
public:
	virtual const std::string& Name(void) const = 0;
	virtual const std::map<std::string, double>& DefaultParams(void) = 0;
	virtual bool ParseParams(const ModelParamsDO& modelParams, void* pParamObj) = 0;
};

#endif