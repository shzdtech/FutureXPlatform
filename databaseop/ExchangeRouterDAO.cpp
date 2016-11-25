#include "ExchangeRouterDAO.h"
#include "MySqlConnectionManager.h"

void ExchangeRouterDAO::FindAllExchangeRouters(std::map<std::string, std::string>& routerMap)
{
	static const std::string sql_findallrouters("SELECT name,address FROM exchange_router");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoCloseStatement_Ptr stmt(session->getConnection()->createStatement());
		AutoCloseResultSet_Ptr rs(stmt->executeQuery(sql_findallrouters));

		while (rs->next())
		{
			routerMap[rs->getString(1)] = rs->getString(2);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}
