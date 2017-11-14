#include "ExchangeRouterTable.h"
#include "../utility/autofillmap.h"
#include "../databaseop/ExchangeRouterDAO.h"

static std::map<std::string, ExchangeRouterDO> routerTable;
const std::string ExchangeRouterTable::TARGET_MD("TARGET.MD.CFG");
const std::string ExchangeRouterTable::TARGET_TD("TARGET.TD.CFG");
const std::string ExchangeRouterTable::TARGET_MD_AM("TARGET.MDAM.CFG");
const std::string ExchangeRouterTable::TARGET_TD_AM("TARGET.TDAM.CFG");
ExchangeRouterTable::static_initializer ExchangeRouterTable::_static_init;

ExchangeRouterTable::static_initializer::static_initializer()
{
	ExchangeRouterDAO::FindAllExchangeRouters(routerTable);
}

bool ExchangeRouterTable::TryFind(const std::string& name, ExchangeRouterDO& address)
{
	auto it = routerTable.find(name);
	if (it == routerTable.end())
		return false;

	address = it->second;

	return true;
}
