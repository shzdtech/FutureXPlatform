#if !defined(__pricingengine_IAlgorithmBase_h)
#define __pricingengine_IAlgorithmBase_h

#include <string>
#include <map>
#include <memory>
#include "../dataobject/ModelParamsDO.h"

class IAlgorithmBase
{
public:
	virtual const std::string& Name(void) const = 0;
	virtual const std::map<std::string, double>& DefaultParams(void) const = 0;
	virtual void ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target) = 0;
};

typedef std::shared_ptr<IAlgorithmBase> IAlgorithmBase_Ptr;

#endif