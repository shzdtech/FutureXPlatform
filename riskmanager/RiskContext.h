#pragma once
#include "IRiskContext.h"
#include "../utility/singleton_templ.h"
#include "riskmanager_exp.h"

class RISKMANAGER_CLASS_EXPORTS RiskContext : public IRiskContext, public singleton_ptr<RiskContext>
{
public:
	RiskContext();

	virtual UserModelKeyMap GetPreTradeUserModel(const std::string& userId);
	virtual UserModelKeyMap InsertPreTradeUserModel(const std::string& userId, const std::string& instanceName);

	virtual UserModelKeyMap GetPostTradeUserModel(const std::string& userId);
	virtual UserModelKeyMap InsertPostTradeUserModel(const std::string& userId, const std::string& instanceName);

private:
	ModelKeyMap _preTradeUserModels;
	ModelKeyMap _postTradeUserModels;
};

