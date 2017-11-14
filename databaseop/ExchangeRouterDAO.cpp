#include "ExchangeRouterDAO.h"
#include "MySqlConnectionManager.h"

void ExchangeRouterDAO::FindAllExchangeRouters(std::map<std::string, ExchangeRouterDO>& routerMap)
{
	static const std::string sql_findallrouters(
		"SELECT name,address,brokerId,user_name,password,"
		"product_info,auth_code "
		"FROM exchange_router");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoCloseStatement_Ptr stmt(session->getConnection()->createStatement());
		AutoCloseResultSet_Ptr rs(stmt->executeQuery(sql_findallrouters));

		while (rs->next())
		{
			ExchangeRouterDO exDO;
			exDO.Name = rs->getString(1);
			exDO.Address = rs->getString(2);
			exDO.BrokeID = rs->getString(3);
			exDO.UserID = rs->getString(4);
			exDO.Password = rs->getString(5);
			exDO.ProductInfo = rs->getString(6);
			exDO.AuthCode = rs->getString(7);

			routerMap[exDO.Name] = exDO;
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}
