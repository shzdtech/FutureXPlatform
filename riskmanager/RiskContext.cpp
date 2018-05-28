#include "RiskContext.h"



RiskContext::RiskContext()
	: _preTradeUserModels(128), _postTradeUserModels(128)
{
}

ModelKeyMap RiskContext::GetPreTradeUserModel(const PortfolioKey& portfolioID)
{
	ModelKeyMap ret;
	_preTradeUserModels.find(portfolioID, ret);
	return ret;
}

ModelKeyMap RiskContext::InsertPreTradeUserModel(const PortfolioKey& portfolioID, const std::string& instanceName)
{
	ModelKeyMap ret;
	if (!_preTradeUserModels.find(portfolioID, ret))
	{
		_preTradeUserModels.insert(portfolioID, ModelKeyMap(2));
		_preTradeUserModels.find(portfolioID, ret);
	}

	ret.map()->insert_or_assign(ModelKey(instanceName, portfolioID.UserID()), true);

	return ret;
}

ModelKeyMap RiskContext::GetPostTradeUserModel(const PortfolioKey& portfolioID)
{
	ModelKeyMap ret;
	_postTradeUserModels.find(portfolioID, ret);
	return ret;
}


ModelKeyMap RiskContext::InsertPostTradeUserModel(const PortfolioKey& portfolioID, const std::string& instanceName)
{
	ModelKeyMap ret;
	if (!_postTradeUserModels.find(portfolioID, ret))
	{
		_postTradeUserModels.insert(portfolioID, ModelKeyMap(2));
		_postTradeUserModels.find(portfolioID, ret);
	}

	ret.map()->insert_or_assign(ModelKey(instanceName, portfolioID.UserID()), true);

	return ret;
}