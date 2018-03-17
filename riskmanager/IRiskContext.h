#pragma once
#include <memory>
#include "../dataobject/TypedefDO.h"

class IRiskContext
{
	virtual UserModelKeyMap GetPreTradeUserModel(const std::string& userId) = 0;
	virtual UserModelKeyMap GetPostTradeUserModel(const std::string& userId) = 0;
};