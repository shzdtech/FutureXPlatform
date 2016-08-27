/***********************************************************************
 * Module:  OrderDAO.cpp
 * Author:  milk
 * Modified: 2015年10月16日 0:28:29
 * Purpose: Implementation of the class OrderDAO
 ***********************************************************************/

#include "OrderDAO.h"
#include "MySqlConnectionManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       OrderDAO::CreateOrder(const OrderDO& orderDO)
// Purpose:    Implementation of OrderDAO::CreateOrder()
// Parameters:
// - orderDO
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool OrderDAO::CreateOrder(OrderRequestDO& orderDO)
{
	bool ret = false;

	static const std::string sql_proc_createorder("insert into exchange_order(accountid,portfolio) values (?,?)");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_createorder));
		prestmt->setString(1, orderDO.UserID());
		prestmt->setString(2, orderDO.PortfolioID());

		prestmt->executeUpdate();

		AutoCloseStatement_Ptr stmt(session->getConnection()->createStatement());
		AutoCloseResultSet_Ptr rsout(stmt->executeQuery("select LAST_INSERT_ID()"));
		if (rsout->next())
		{
			orderDO.OrderID = rsout->getUInt64(1);
		}

		ret = true;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

bool OrderDAO::QueryAllOrderPortfolio(std::map<uint64_t, std::string>& memoryPortfolio)
{
	bool ret = false;

	static const std::string sql_findportfolio(
		"SELECT id,portfolio FROM exchange_order");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findportfolio));

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			memoryPortfolio.emplace(rs->getUInt64(1), rs->getString(2));
		}

		ret = true;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}
