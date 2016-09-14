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

VectorDO_Ptr<StrategyContractDO> StrategyContractDAO::FindStrategyContractByUser(
	const std::string& userid, int productType)
{
	static const std::string sql_findstrategy(
		"SELECT exchange_symbol, contract_symbol, underlying_symbol, tick_size, multiplier, "
		"strategy_symbol, descript, portfolio_symbol, contract_type, "
		"strikeprice, expiration, accountid, is_trading_allowed, product_type "
		"FROM vw_strategy_contract_info "
		"WHERE accountid like ? and product_type = ?");

	auto ret = std::make_shared<VectorDO<StrategyContractDO>>();
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findstrategy));
		prestmt->setString(1, userid.empty() ? "%" : userid);
		prestmt->setInt(2, productType);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		auto now = std::time(nullptr);
		auto pTM = std::localtime(&now);

		while (rs->next())
		{
			auto accountid = rs->getString(12);
			StrategyContractDO stcdo(rs->getString(1), rs->getString(2), accountid, rs->getString(8));
			stcdo.TradingDay.Year = pTM->tm_year + 1900;
			stcdo.TradingDay.Month = pTM->tm_mon + 1;
			stcdo.TradingDay.Day = pTM->tm_mday;
			stcdo.Underlying = rs->getString(3);
			stcdo.TickSize = rs->getDouble(4);
			stcdo.Multiplier = rs->getDouble(5);
			stcdo.StrategyName = rs->getString(6);
			stcdo.Description = rs->getString(7);
			stcdo.ContractType = (ContractType)rs->getInt(9);
			stcdo.StrikePrice = rs->getDouble(10);
			if (!rs->isNull(11))
			{
				std::string strDate = rs->getString(11);
				stcdo.Expiration = DateType(strDate);
			}
			stcdo.Enabled = rs->getBoolean(13);
			stcdo.ProductType = (ProductType)rs->getInt(14);

			RetrievePricingContracts(stcdo.ExchangeID(), stcdo.InstrumentID(), accountid, stcdo.PricingContracts);
			RetrieveStrategyModels(stcdo.StrategyName, accountid, stcdo);

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


void StrategyContractDAO::RetrievePricingContracts(const std::string& strategyExchange, const std::string& strategyContract, const std::string& userid,
std::vector<PricingContract>& pricingContracts)
{
	static const std::string sql_findcontractparam(
		"SELECT pricing_exchange, pricing_contract, weight FROM strategy_pricing_contract "
		"WHERE strategy_exchange = ? and strategy_contract = ? and accountid = ?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontractparam));
		prestmt->setString(1, strategyExchange);
		prestmt->setString(2, strategyContract);
		prestmt->setString(3, userid);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			PricingContract cp(rs->getString(1), rs->getString(2), rs->getDouble(3));
			pricingContracts.push_back(std::move(cp));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

void StrategyContractDAO::RetrieveStrategyModels(const std::string & strategySym, const std::string & userid, StrategyContractDO& sto)
{
	std::string modelInstance;
	static const std::string PM("pm");
	if (FindStrategyModelByAim(strategySym, PM, userid, modelInstance))
	{
		sto.PricingModel = std::make_shared<ModelParamsDO>(modelInstance, "", userid);
	}

	static const std::string IVM("ivm");
	if (FindStrategyModelByAim(strategySym, IVM, userid, modelInstance))
	{
		sto.IVModel = std::make_shared<ModelParamsDO>(modelInstance, "", userid);
	}

	static const std::string VM("vm");
	if (FindStrategyModelByAim(strategySym, VM, userid, modelInstance))
	{
		sto.VolModel = std::make_shared<ModelParamsDO>(modelInstance, "", userid);
	}
}


bool StrategyContractDAO::FindStrategyModelByAim(const std::string & strategySym, const std::string & aim, const std::string & userid, std::string & modelinstance)
{
	static const std::string sql_findstrategymodel(
		"SELECT modelinstance "
		"FROM strategy_model "
		"WHERE accountid = ? and strategy_symbol = ? and modelaim = ?");

	bool ret = false;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findstrategymodel));
		prestmt->setString(1, userid);
		prestmt->setString(2, strategySym);
		prestmt->setString(3, aim);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		if (rs->next())
		{
			modelinstance = rs->getString(1);
			ret = true;
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

VectorDO_Ptr<ContractParamDO> StrategyContractDAO::RetrieveContractParamByUser(const std::string& userid)
{
	static const std::string sql_findcontractparam(
		"SELECT distinct pricing_exchange, pricing_contract, tick_size, multiplier "
		"FROM vw_pricing_contract_property "
		"WHERE accountid = ?");

	auto ret = std::make_shared<VectorDO<ContractParamDO>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontractparam));
		prestmt->setString(1, userid);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ContractParamDO cdo(rs->getString(1), rs->getString(2));
			cdo.TickSize = rs->getDouble(3);
			cdo.Multiplier = rs->getDouble(4);
			ret->push_back(std::move(cdo));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}