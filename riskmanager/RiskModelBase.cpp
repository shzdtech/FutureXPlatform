#include "RiskModelBase.h"



RiskModelBase::RiskModelBase()
{
}


RiskModelBase::~RiskModelBase()
{
}

const std::map<std::string, double>& RiskModelBase::DefaultParams(void) const
{
	static std::map<std::string, double> defaultParams = {
		{ RiskModelParams::__action__ , RiskModelParams::ActionType::NO_ACTION },
		{ RiskModelParams::__enabled__ , 1 },
		{ RiskModelParams::__interval__ , 0 },
		{ RiskModelParams::__match__ , 0 },
		{ RiskModelParams::__type__ , 0 },
	};
	return defaultParams;
}

void RiskModelBase::ParseParams(const std::map<std::string, double>& modelParams, std::unique_ptr<ParamsBase>& target)
{
	if (!target)
		target = std::make_unique<RiskModelParams>();

	auto pParam = (RiskModelParams*)target.get();

	auto& defaultParams = DefaultParams();
	auto endit = modelParams.end();

	auto it = modelParams.find(RiskModelParams::__action__);
	pParam->Action = (RiskModelParams::ActionType)(int)(it == endit ? defaultParams.at(RiskModelParams::__action__) : it->second);

	it = modelParams.find(RiskModelParams::__enabled__);
	pParam->Enabled = (bool)(it == endit ? defaultParams.at(RiskModelParams::__enabled__) : it->second);

	it = modelParams.find(RiskModelParams::__interval__);
	pParam->Interval = (int)(it == endit ? defaultParams.at(RiskModelParams::__interval__) : it->second);

	it = modelParams.find(RiskModelParams::__match__);
	pParam->MatchAny = std::round(it == endit ? defaultParams.at(RiskModelParams::__match__) : it->second) == 0;
	
	it = modelParams.find(RiskModelParams::__type__);
	pParam->PreTrade = std::round(it == endit ? defaultParams.at(RiskModelParams::__type__) : it->second) == 0;
}
