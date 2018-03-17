#pragma once
#include "IRiskModelAlgorithm.h"
#include "RiskModelParams.h"
#include "../message/BizError.h"

class RiskModelBase : public IRiskModelAlgorithm
{
public:
	RiskModelBase();
	~RiskModelBase();

	virtual const std::map<std::string, double>& DefaultParams(void) const;
	virtual void ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target);
};

