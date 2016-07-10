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

VectorDO_Ptr<PortfolioDO> PortfolioDAO::FindPortfolioByUser(const std::string& userID)
{
	static const std::string sql_findportfolio(
		"SELECT portfolio_symbol, hedge_delay FROM portfolio "
		"WHERE client_account = ?");

	auto ret = std::make_shared<VectorDO<PortfolioDO>>();
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findportfolio));
		prestmt->setString(1, userID);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			PortfolioDO pfdo(rs->getString(1), userID);
			pfdo.HedgeDelay = rs->getInt(2);

			ret->push_back(std::move(pfdo));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LiteLogger::Error(std::string(__FUNCTION__) + ": " + sqlEx.getSQLStateCStr());
		throw BizError(DB_ERROR, sqlEx.getSQLStateCStr(), sqlEx.getErrorCode());
	}

	return ret;
}