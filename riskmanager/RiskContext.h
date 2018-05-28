#pragma once
#include "IRiskContext.h"
#include "../utility/singleton_templ.h"
#include "riskmanager_exp.h"

class RISKMANAGER_CLASS_EXPORTS RiskContext : public IRiskContext, public singleton_ptr<RiskContext>
{
public:
	RiskContext();

	virtual ModelKeyMap GetPreTradeUserModel(const PortfolioKey& portfolioID);
	virtual ModelKeyMap InsertPreTradeUserModel(const PortfolioKey& portfolioID, const std::string& instanceName);

	virtual ModelKeyMap GetPostTradeUserModel(const PortfolioKey& portfolioID);
	virtual ModelKeyMap InsertPostTradeUserModel(const PortfolioKey& portfolioID, const std::string& instanceName);

private:
	PortfolioModelKeyMap _preTradeUserModels;
	PortfolioModelKeyMap _postTradeUserModels;
};

