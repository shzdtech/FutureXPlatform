#include "PositionPortfolioMap.h"
#include "../dataobject/TemplateDO.h"
#include "../databaseop/PortfolioDAO.h"

static bool _initialized = false;

static std::map<std::string, PortfolioKey> _portfolios;

PortfolioKey * PositionPortfolioMap::FindPortfolio(const std::string & instumentId)
{
	PortfolioKey * ret = nullptr;

	if (!_initialized)
	{
		PortfolioDAO::QueryDefaultPortfolio(_portfolios);
		_initialized = true;
	}

	auto it = _portfolios.find(instumentId);

	if (it != _portfolios.end())
		ret = &it->second;

	return ret;
}
