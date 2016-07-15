/***********************************************************************
 * Module:  StrategyConstractDAO.cpp
 * Author:  milk
 * Modified: 2015年8月8日 22:30:32
 * Purpose: Implementation of the class StrategyConstractDAO
 ***********************************************************************/

#include "StrategyContractDAO.h"
#include "MySqlConnectionManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       StrategyConstractDAO::FindStrategyContractByKey(const ContractKey& contractID)
// Purpose:    Implementation of StrategyConstractDAO::FindStrategyContractByKey()
// Parameters:
// - contractID
// Return:     std::shared_ptr<std::vector<StrategyContractDO>>
////////////////////////////////////////////////////////////////////////

VectorDO_Ptr<StrategyContractDO> StrategyContractDAO::
FindStrategyContractByClient(const std::string& clientSymbol)
{
	static const std::string sql_findstrategy(
		"SELECT exchange_symbol, contract_symbol, underlying_symbol, tick_size, multiplier, "
		"strategy_symbol, descript, pricing_algorithm, portfolio_symbol "
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

			stcdo.ParamMap = FindStrategyParam(stcdo.Strategy);
			stcdo.BaseContracts = FindContractParam(stcdo.Strategy);

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


VectorDO_Ptr<ContractParam> StrategyContractDAO::FindContractParam(const std::string& strategy)
{
	static const std::string sql_findcontractparam(
		"SELECT exchange_symbol, contract_symbol, weight FROM vw_strategy_param "
		"WHERE strategy_symbol = ?");

	auto ret = std::make_shared<VectorDO<ContractParam>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontractparam));
		prestmt->setString(1, strategy);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ContractParam cp(rs->getString(1), rs->getString(2));
			cp.Weight = rs->getDouble(3);
			cp.Beta = cp.Weight;

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

std::shared_ptr<std::map<std::string, double>> StrategyContractDAO::FindStrategyParam(const std::string& strategy)
{
	static const std::string sql_findstrategyparam(
		"SELECT param_name, param_value FROM strategy_pricing_param "
		"WHERE strategy_symbol = ?");

	auto ret = std::make_shared<std::map<std::string, double>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findstrategyparam));
		prestmt->setString(1, strategy);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ret->emplace(rs->getString(1), rs->getDouble(2));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}