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

VectorDO_Ptr<StrategyContractDO> StrategyContractDAO::FindStrategyContractByClient(
	const std::string& clientSymbol, int productType)
{
	static const std::string sql_findstrategy(
		"SELECT exchange_symbol, contract_symbol, underlying_symbol, tick_size, multiplier, "
		"strategy_symbol, descript, pricing_algorithm, portfolio_symbol, contract_type, strikeprice, expiration, client_symbol "
		"FROM vw_strategy_contract_info "
		"WHERE client_symbol like ? and product_type = ?");

	auto ret = std::make_shared<VectorDO<StrategyContractDO>>();
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findstrategy));
		prestmt->setString(1, clientSymbol.length() ? clientSymbol : "%");
		prestmt->setInt(2, productType);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		auto now = std::time(nullptr);
		auto pTM = std::localtime(&now);

		while (rs->next())
		{
			auto clientSym = rs->getString(13);
			StrategyContractDO stcdo(rs->getString(1), rs->getString(2), clientSym, rs->getString(9));
			stcdo.TradingDay.Year = pTM->tm_year + 1900;
			stcdo.TradingDay.Month = pTM->tm_mon + 1;
			stcdo.TradingDay.Day = pTM->tm_mday;
			stcdo.Underlying = rs->getString(3);
			stcdo.TickSize = rs->getDouble(4);
			stcdo.Multiplier = rs->getDouble(5);
			stcdo.StrategyName = rs->getString(6);
			stcdo.Description = rs->getString(7);
			stcdo.ModelParams.ModelName = rs->getString(8);
			stcdo.ContractType = (ContractType)rs->getInt(10);
			stcdo.StrikePrice = rs->getDouble(11);
			if (!rs->isNull(12))
			{
				std::string strDate = rs->getString(12);
				std::sscanf(strDate.data(), "%d-%d-%d", 
					&stcdo.Expiration.Year, &stcdo.Expiration.Month, &stcdo.Expiration.Day);
			}

			RetrieveModelParams(stcdo.StrategyName, clientSym, stcdo.ModelParams);
			RetrievePricingContracts(stcdo.StrategyName, clientSym, stcdo.PricingContracts);

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


void StrategyContractDAO::RetrievePricingContracts
(const std::string& strategySymbol, const std::string& clientSymbol, std::vector<PricingContract>& pricingContracts)
{
	static const std::string sql_findcontractparam(
		"SELECT exchange_symbol, contract_symbol, weight FROM vw_strategy_pricingcontracts "
		"WHERE strategy_symbol = ? and client_symbol = ?");

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

			pricingContracts.push_back(std::move(cp));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

void StrategyContractDAO::RetrieveModelParams(const std::string& strategySymbol, const std::string& clientSymbol,
	ModelParamsDO& modelParams)
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
			modelParams.ScalaParams[rs->getString(1)] = rs->getDouble(2);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}