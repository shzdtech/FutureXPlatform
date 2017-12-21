#pragma once

#include "../dataobject/ContractKey.h"
#include "../dataobject/PortfolioDO.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT PositionPortfolioMap
{
public:
	static PortfolioKey* FindPortfolio(const std::string & userId,
		const std::string& instumentId);

	static bool LoadUserPortfolio(const std::string & userId);
};

