#include "RiskContext.h"



RiskContext::RiskContext()
	: _preTradeUserModels(128), _postTradeUserModels(128)
{
}

UserModelKeyMap RiskContext::GetPreTradeUserModel(const std::string& userId)
{
	UserModelKeyMap ret;
	_preTradeUserModels.find(userId, ret);
	return ret;
}

UserModelKeyMap RiskContext::InsertPreTradeUserModel(const std::string& userId, const std::string& instanceName)
{
	UserModelKeyMap ret;
	if (!_preTradeUserModels.find(userId, ret))
	{
		_preTradeUserModels.insert(userId, UserModelKeyMap(4));
		_preTradeUserModels.find(userId, ret);
	}

	ret.map()->insert_or_assign(instanceName, ModelKey(instanceName, userId));

	return ret;
}

UserModelKeyMap RiskContext::GetPostTradeUserModel(const std::string& userId)
{
	UserModelKeyMap ret;
	_postTradeUserModels.find(userId, ret);
	return ret;
}


UserModelKeyMap RiskContext::InsertPostTradeUserModel(const std::string& userId, const std::string& instanceName)
{
	UserModelKeyMap ret;
	if (!_postTradeUserModels.find(userId, ret))
	{
		_postTradeUserModels.insert(userId, UserModelKeyMap(4));
		_postTradeUserModels.find(userId, ret);
	}

	ret.map()->insert_or_assign(instanceName, ModelKey(instanceName, userId));

	return ret;
}