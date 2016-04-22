/***********************************************************************
 * Module:  PortfolioDAO.cpp
 * Author:  milk
 * Modified: 2016年2月7日 22:10:21
 * Purpose: Implementation of the class PortfolioDAO
 ***********************************************************************/

#include "PortfolioDAO.h"
#include "ConnectionHelper.h"

////////////////////////////////////////////////////////////////////////
// Name:       PortfolioDAO::FindPortfolioByUser(const std::string& userID)
// Purpose:    Implementation of PortfolioDAO::FindPortfolioByUser()
// Parameters:
// - userID
// Return:     int
////////////////////////////////////////////////////////////////////////

static const std::string sql_findportfolio(
	"SELECT portfolio_symbol, hedge_delay FROM portfolio "
	"WHERE client_account = ?");

VectorDO_Ptr<PortfolioDO> PortfolioDAO::FindPortfolioByUser(const std::string& userID)
{
	auto ret = std::make_shared<VectorDO<PortfolioDO>>();
	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
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
	catch (std::exception& ex)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << ex.what();
	}

	return ret;
}