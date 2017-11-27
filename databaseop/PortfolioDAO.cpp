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

VectorDO_Ptr<PortfolioDO> PortfolioDAO::FindPortfolios(const std::string userId)
{
	static const std::string sql_findportfolio(
		"SELECT accountid, portfolio_symbol, hedge_delay, hedge_threshold FROM portfolio WHERE accountid like ?");

	std::map<PortfolioKey, std::pair<std::string, ContractKey>> pricingContractMap;
	RetrieveHedgeContracts(pricingContractMap);

	auto ret = std::make_shared<VectorDO<PortfolioDO>>();
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findportfolio));

		prestmt->setString(1, userId.empty() ? "%" : userId);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			PortfolioDO pfdo(rs->getString(2), rs->getString(1));
			pfdo.HedgeDelay = rs->getInt(3);
			pfdo.Threshold = rs->getDouble(4);

			auto it = pricingContractMap.find(pfdo);
			if (it != pricingContractMap.end())
			{
				pfdo.HedgeContracts.emplace(it->second.first, std::make_shared<ContractKey>(it->second.second));
			}

			ret->push_back(std::move(pfdo));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

int PortfolioDAO::UpsertPortofolio(const PortfolioDO & portfolio)
{
	UpsertPortofolioSettings(portfolio);
	UpsertHedgeContracts(portfolio);
	return 0;
}

int PortfolioDAO::UpsertPortofolioSettings(const PortfolioDO & portfolio)
{
	static const std::string sql_newportfolio(
		"INSERT INTO portfolio (accountid, portfolio_symbol, hedge_delay, hedge_threshold) "
		"VALUES (?,?,?,?) ON DUPLICATE KEY UPDATE hedge_delay = ?, hedge_threshold = ? ");

	auto ret = -1;
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_newportfolio));
		prestmt->setString(1, portfolio.UserID());
		prestmt->setString(2, portfolio.PortfolioID());
		prestmt->setInt(3, portfolio.HedgeDelay);
		prestmt->setDouble(4, portfolio.Threshold);
		prestmt->setInt(5, portfolio.HedgeDelay);
		prestmt->setDouble(6, portfolio.Threshold);

		prestmt->executeUpdate();

		ret = 0;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}


int PortfolioDAO::UpsertHedgeContracts(const PortfolioDO & portfolio)
{
	static const std::string sql_upsert_hedge_ontracts(
		"INSERT INTO portfolio_hedging_contract (accountid, portfolio_symbol, hedging_underlying, hedging_exchange, hedging_contract )"
		"VALUES (?,?,?,?,?) ON DUPLICATE KEY UPDATE hedging_contract = ?");

	auto ret = -1;
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_upsert_hedge_ontracts));
		prestmt->setString(1, portfolio.UserID());
		prestmt->setString(2, portfolio.PortfolioID());

		for (auto pair : portfolio.HedgeContracts)
		{
			prestmt->setString(3, pair.first);
			prestmt->setString(4, pair.second->ExchangeID());
			prestmt->setString(5, pair.second->InstrumentID());
			prestmt->setString(6, pair.second->InstrumentID());
			prestmt->executeUpdate();
		}

		ret = 0;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}


void PortfolioDAO::RetrieveHedgeContracts(std::map<PortfolioKey, std::pair<std::string, ContractKey>>& pricingContractMap)
{
	static const std::string sql_retrieve_hedgecontracts(
		"SELECT accountid,portfolio_symbol,hedging_underlying,hedging_exchange,hedging_contract "
		"FROM portfolio_hedging_contract ");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_retrieve_hedgecontracts));

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			pricingContractMap.emplace(PortfolioKey(rs->getString(2), rs->getString(1)),
				std::make_pair(rs->getString(3), ContractKey(rs->getString(4), rs->getString(5))));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

bool PortfolioDAO::QueryDefaultPortfolio(std::map<std::string, PortfolioKey>& portfolios)
{
	bool ret = false;

	static const std::string sql_findPortfolios(
		"SELECT exchange_symbol, contract_symbol, portfolio_symbol, accountid "
		"FROM vw_portfolio_contract ");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findPortfolios));

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			portfolios.emplace(rs->getString(2), PortfolioKey(rs->getString(3), rs->getString(4)));
		}

		ret = true;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}