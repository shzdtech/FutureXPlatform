/***********************************************************************
 * Module:  PortfolioDAO.cpp
 * Author:  milk
 * Modified: 2016年2月7日 22:10:21
 * Purpose: Implementation of the class PortfolioDAO
 ***********************************************************************/

#include "PortfolioDAO.h"
#include "MySqlConnectionManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       PortfolioDAO::FindPortfolioByUser(const std::string& userID)
// Purpose:    Implementation of PortfolioDAO::FindPortfolioByUser()
// Parameters:
// - userID
// Return:     int
////////////////////////////////////////////////////////////////////////

VectorDO_Ptr<PortfolioDO> PortfolioDAO::FindPortfolioByUser(const std::string& userid)
{
	static const std::string sql_findportfolio(
		"SELECT portfolio_symbol, hedge_delay FROM portfolio "
		"WHERE accountid = ?");

	auto ret = std::make_shared<VectorDO<PortfolioDO>>();
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findportfolio));
		prestmt->setString(1, userid);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			PortfolioDO pfdo(rs->getString(1), userid);
			pfdo.HedgeDelay = rs->getInt(2);

			ret->push_back(std::move(pfdo));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

int PortfolioDAO::CreatePortofolio(const PortfolioDO & portfolio)
{
	static const std::string sql_newportfolio(
		"insert into portfolio (accountid, portfolio_symbol, hedge_delay, descript) "
		"values (?,?,?,?)");

	auto ret = -1;
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_newportfolio));
		prestmt->setString(1, portfolio.UserID());
		prestmt->setString(2, portfolio.PortfolioID());
		prestmt->setInt(3, portfolio.HedgeDelay);
		prestmt->setString(4, portfolio.PortfolioID());

		prestmt->executeUpdate();

		ret = 0;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}
