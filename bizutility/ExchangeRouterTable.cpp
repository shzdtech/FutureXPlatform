#include "ExchangeRouterTable.h"
#include "libcuckoo/cuckoohash_map.hh"
#include "../databaseop/ExchangeRouterDAO.h"

static cuckoohash_map<std::string, std::string> routerTable(128);
const std::string ExchangeRouterTable::TARGET_MD("md");
const std::string ExchangeRouterTable::TARGET_TD("td");
ExchangeRouterTable::static_initializer ExchangeRouterTable::_static_init;

ExchangeRouterTable::static_initializer::static_initializer()
{
	std::map<std::string, std::string> exchangeTable;
	ExchangeRouterDAO::FindAllExchangeRouters(exchangeTable);
	for (auto& pair : exchangeTable)
		routerTable.insert(pair.first, pair.second);
}

bool ExchangeRouterTable::TryFind(const std::string& name, std::string& address)
{
	return routerTable.find(name, address);
}
