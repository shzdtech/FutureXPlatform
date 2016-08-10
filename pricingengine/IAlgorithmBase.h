#if !defined(__pricingengine_IAlgorithmBase_h)
#define __pricingengine_IAlgorithmBase_h

#include <string>
#include <map>

class IAlgorithmBase
{
public:
	virtual const std::string& Name(void) const = 0;
	virtual const std::map<std::string, double>& DefaultParams(void) = 0;
	virtual bool ParseParams(const std::map<std::string, double>& params, void* pParamObj) = 0;
};

#endif