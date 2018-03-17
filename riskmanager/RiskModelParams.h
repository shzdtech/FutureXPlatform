#if !defined(__riskmanager_RiskModelParams_h)
#define __riskmanager_RiskModelParams_h

#include <string>
#include "../dataobject/ParamsBase.h"

class RiskModelParams : public ParamsBase
{
public:
	enum ActionType { NO_ACTION = 0, WARNING = 1, STOP_OPEN_ORDER = 2 };

public:
	ActionType Action;
	bool Enabled = true;
	bool MatchAny;
	int Interval;
	bool PreTrade = true;

public:
	static const std::string __action__;
	static const std::string __enabled__;
	static const std::string __interval__;
	static const std::string __match__;
	static const std::string __type__;
};

#endif