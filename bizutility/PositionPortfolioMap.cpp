#include "PositionPortfolioMap.h"
#include "../dataobject/TemplateDO.h"
#include "../databaseop/PortfolioDAO.h"
#include "../include/libcuckoo/cuckoohash_map.hh"


static cuckoohash_map<std::string, map_ptr<std::string, PortfolioKey>> _portfolios;

PortfolioKey * PositionPortfolioMap::FindPortfolio(const std::string & userId, const std::string & instumentId)
{
	PortfolioKey * ret = nullptr;

	map_ptr<std::string, PortfolioKey> userProfileMap;
	if (_portfolios.find(userId, userProfileMap))
	{
		auto it = userProfileMap->find(instumentId);

		if (it != userProfileMap->end())
			ret = &it->second;
	}

	return ret;
}

bool PositionPortfolioMap::LoadUserPortfolio(const std::string & userId)
{
	bool ret = true;

	if (!_portfolios.contains(userId))
	{
		auto userProfileMap = std::make_shared<std::map<std::string, PortfolioKey>>();
		if (ret = PortfolioDAO::QueryDefaultPortfolio(userId, *userProfileMap))
			_portfolios.insert(userId, userProfileMap);
	}

	return ret;
}
