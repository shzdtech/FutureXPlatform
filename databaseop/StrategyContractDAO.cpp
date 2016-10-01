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

VectorDO_Ptr<StrategyContractDO> StrategyContractDAO::LoadStrategyContractByProductType(int productType)
{
	static const std::string sql_findstrategy(
		"SELECT exchange_symbol, contract_symbol, underlying_symbol, tick_size, multiplier, "
		"strategy_symbol, descript, portfolio_symbol, contract_type, "
		"strikeprice, expiration, accountid, product_type, bid_allowed, ask_allowed "
		"FROM vw_strategy_contract_info "
		"WHERE product_type = ?");

	VectorDO_Ptr<StrategyContractDO> ret = std::make_shared<VectorDO<StrategyContractDO>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		autofillmap<UserContractKey, std::vector<PricingContract>> pricingContractMap;
		RetrievePricingContractsByProductType(productType, pricingContractMap);

		autofillmap<UserStrategyName, autofillmap<std::string, std::string>> strategyDOMap;
		RetrieveStrategyModels(strategyDOMap);

		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findstrategy));
		prestmt->setInt(1, productType);

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
			
			stcdo.ProductType = (ProductType)rs->getInt(13);
			stcdo.BidEnabled = rs->getBoolean(14);
			stcdo.AskEnabled = rs->getBoolean(15);

			stcdo.PricingContracts = pricingContractMap.getorfill(stcdo);
			auto& modelMap = strategyDOMap.getorfill(UserStrategyName(stcdo.UserID(), stcdo.StrategyName));

			std::string modelInstance;
			static const std::string PM("pm");
			if (modelMap.tryfind(PM, modelInstance))
				stcdo.PricingModel = std::make_shared<ModelParamsDO>(modelInstance, "", stcdo.UserID());

			static const std::string IVM("ivm");
			if (modelMap.tryfind(IVM, modelInstance))
				stcdo.IVModel = std::make_shared<ModelParamsDO>(modelInstance, "", stcdo.UserID());

			static const std::string VM("vm");
			if (modelMap.tryfind(VM, modelInstance))
				stcdo.VolModel = std::make_shared<ModelParamsDO>(modelInstance, "", stcdo.UserID());

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


void StrategyContractDAO::RetrievePricingContractsByProductType(int productType, autofillmap<UserContractKey, std::vector<PricingContract>>& pricingContractMap)
{
	static const std::string sql_findcontractparam(
		"SELECT accountid, strategy_exchange, strategy_contract, pricing_exchange, pricing_contract, weight from vw_pricing_contract_property "
		"WHERE strategy_product_type = ?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontractparam));
		prestmt->setInt(1, productType);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			auto& pricingVector = pricingContractMap.getorfill(UserContractKey(rs->getString(2), rs->getString(3), rs->getString(1)));
			PricingContract cp(rs->getString(4), rs->getString(5), rs->getDouble(6));
			pricingVector.push_back(std::move(cp));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

void StrategyContractDAO::RetrieveStrategyModels(autofillmap<UserStrategyName, autofillmap<std::string, std::string>>& strategyDOMap)
{
	static const std::string sql_findstrategymodel(
		"SELECT accountid, strategy_symbol, modelaim, modelinstance FROM strategy_model ");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findstrategymodel));

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			strategyDOMap.getorfill(UserStrategyName(rs->getString(1), rs->getString(2))).emplace(rs->getString(3), rs->getString(4));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
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

void StrategyContractDAO::UpdateStrategy(const StrategyContractDO & strategyDO)
{
	static const std::string sql_savepStrategy(
		"CALL Strategy_Upsert(?,?,?,?,?,?,?,?,?,?)");

	ModelParamsDO_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(session->getConnection()->prepareStatement(sql_savepStrategy));
		prestmt->setString(1, strategyDO.UserID());
		prestmt->setString(2, strategyDO.ExchangeID());
		prestmt->setString(3, strategyDO.InstrumentID());
		prestmt->setString(4, strategyDO.PortfolioID());
		prestmt->setBoolean(5, strategyDO.BidEnabled);
		prestmt->setBoolean(6, strategyDO.AskEnabled);
		prestmt->setString(7, strategyDO.StrategyName);
		strategyDO.PricingModel ? prestmt->setString(8, strategyDO.PricingModel->InstanceName) : prestmt->setNull(8, 0);
		strategyDO.IVModel ? prestmt->setString(9, strategyDO.IVModel->InstanceName) : prestmt->setNull(9, 0);
		strategyDO.VolModel ? prestmt->setString(10, strategyDO.VolModel->InstanceName) : prestmt->setNull(10, 0);

		prestmt->executeUpdate();
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
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