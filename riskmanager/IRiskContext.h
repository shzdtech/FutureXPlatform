#pragma once
#include <memory>
#include "../dataobject/TypedefDO.h"

class IRiskContext
{
	virtual ModelKeyMap GetPreTradeUserModel(const PortfolioKey& portfolioID) = 0;
	virtual ModelKeyMap GetPostTradeUserModel(const PortfolioKey& portfolioID) = 0;
};