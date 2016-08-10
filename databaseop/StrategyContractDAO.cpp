/***********************************************************************
 * Module:  StrategyConstractDAO.cpp
 * Author:  milk
 * Modified: 2015年8月8日 22:30:32
 * Purpose: Implementation of the class StrategyConstractDAO
 ***********************************************************************/

#include "StrategyContractDAO.h"
#include "MySqlConnectionManager.h"
#include <ctime>

////////////////////////////////////////////////////////////////////////
// Name:       StrategyConstractDAO::FindStrategyContractByKey(const ContractKey& contractID)
// Purpose:    Implementation of StrategyConstractDAO::FindStrategyContractByKey()
// Parameters:
// - contractID
// Return:     std::shared_ptr<std::vector<StrategyContractDO>>
////////////////////////////////////////////////////////////////////////

VectorDO_Ptr<StrategyContractDO> StrategyContractDAO::FindStrategyContractByClient(const std::string& clientSymbol)
{
	static const std::string sql_findstrategy(
		"SELECT exchange_symbol, contract_symbol, underlying_symbol, tick_size, multiplier, "
		"strategy_symbol, descript, pricing_algorithm, portfolio_symbol, expiration "
		"FROM vw_strategy_contract_info "
		"WHERE client_symbol = ?");

	auto ret = std::make_shared<VectorDO<StrategyContractDO>>();
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findstrategy));
		prestmt->setString(1, clientSymbol);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			StrategyContractDO stcdo(rs->getString(1), rs->getString(2), clientSymbol, rs->getString(9));
			stcdo.Underlying = rs->getString(3);
			stcdo.TickSize = rs->getDouble(4);
			stcdo.Multiplier = rs->getDouble(5);
			stcdo.Strategy = rs->getString(6);
			stcdo.Description = rs->getString(7);
			stcdo.Algorithm = rs->getString(8);
			std::time_t time = rs->getInt64(10);
			stcdo.Expiration = *std::localtime(&time);

			FindStrategyParams(stcdo.Strategy, clientSymbol, stcdo.Params);
			stcdo.PricingContracts = FindPricingContracts(stcdo.Strategy, clientSymbol);

			ret->push_back(std::move(stcdo));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}


VectorDO_Ptr<PricingContract> StrategyContractDAO::FindPricingContracts(const std::string& strategySymbol, const std::string& clientSymbol)
{
	static const std::string sql_findcontractparam(
		"SELECT exchange_symbol, contract_symbol, weight FROM vw_strategy_pricingcontracts "
		"WHERE strategy_symbol = ? and client_symbol = ?");

	auto ret = std::make_shared<VectorDO<PricingContract>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontractparam));
		prestmt->setString(1, strategySymbol);
		prestmt->setString(2, clientSymbol);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			PricingContract cp(rs->getString(1), rs->getString(2));
			cp.Weight = rs->getDouble(3);

			ret->push_back(std::move(cp));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

void StrategyContractDAO::FindStrategyParams(const std::string& strategySymbol, const std::string& clientSymbol,
	std::map<std::string, double>& paramMap)
{
	static const std::string sql_findstrategyparam(
		"SELECT param_name, param_value FROM strategy_pricing_param "
		"WHERE strategy_symbol = ? and client_symbol = ?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findstrategyparam));
		prestmt->setString(1, strategySymbol);
		prestmt->setString(2, clientSymbol);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			paramMap[rs->getString(1)] = rs->getDouble(2);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}