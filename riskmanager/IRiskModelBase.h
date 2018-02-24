#if !defined(__riskmanager_IRiskModelBase_h)
#define __riskmanager_IRiskModelBase_h

#include <string>
#include <map>
#include <memory>
#include "../dataobject/ModelParamsDO.h"

class IRiskModelBase
{
public:
	virtual const std::string& Name(void) const = 0;
	virtual const std::map<std::string, double>& DefaultParams(void) const = 0;
	virtual void ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target) = 0;
};

typedef std::shared_ptr<IRiskModelBase> IRiskModelBase_Ptr;

#endif