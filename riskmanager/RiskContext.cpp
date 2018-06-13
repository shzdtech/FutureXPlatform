#include "RiskContext.h"



RiskContext::RiskContext()
	: _preTradeUserModels(128), _postTradeUserModels(128), _preTradeAccountModels(128), _postTradeAccountModels(128)
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

ModelKeyMap RiskContext::GetPreTradeAccountModel(const std::string& userID)
{
	ModelKeyMap ret;
	_preTradeAccountModels.find(userID, ret);
	return ret;
}

ModelKeyMap RiskContext::InsertPreTradeAccountModel(const std::string& userID, const std::string& instanceName)
{
	ModelKeyMap ret;
	if (!_preTradeAccountModels.find(userID, ret))
	{
		_preTradeAccountModels.insert(userID, ModelKeyMap(2));
		_preTradeAccountModels.find(userID, ret);
	}

	ret.map()->insert_or_assign(ModelKey(instanceName, userID), true);

	return ret;
}

ModelKeyMap RiskContext::GetPostTradeAccountModel(const std::string& userID)
{
	ModelKeyMap ret;
	_postTradeAccountModels.find(userID, ret);
	return ret;
}

ModelKeyMap RiskContext::InsertPostTradeAccountModel(const std::string& userID, const std::string& instanceName)
{
	ModelKeyMap ret;
	if (!_postTradeAccountModels.find(userID, ret))
	{
		_postTradeAccountModels.insert(userID, ModelKeyMap(2));
		_postTradeAccountModels.find(userID, ret);
	}

	ret.map()->insert_or_assign(ModelKey(instanceName, userID), true);

	return ret;
}