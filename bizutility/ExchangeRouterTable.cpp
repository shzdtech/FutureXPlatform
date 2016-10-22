#include "ExchangeRouterTable.h"
#include "../databaseop/ExchangeRouterDAO.h"

static autofillmap<std::string, std::string> routerTable;
const std::string ExchangeRouterTable::TARGET_MD("md");
const std::string ExchangeRouterTable::TARGET_TD("td");
ExchangeRouterTable::static_initializer ExchangeRouterTable::_static_init;

ExchangeRouterTable::static_initializer::static_initializer()
{
	ExchangeRouterDAO::FindAllExchangeRouters(routerTable);
}

bool ExchangeRouterTable::TryFind(const std::string& name, std::string& address)
{
	if (auto pAddr = routerTable.tryfind(name))
	{
		address = *pAddr;
		return true;
	}

	return false;
}
