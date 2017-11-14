/***********************************************************************
 * Module:  StrategyConstractDAO.cpp
 * Author:  milk
 * Modified: 2015年8月8日 22:30:32
 * Purpose: Implementation of the class StrategyConstractDAO
 ***********************************************************************/

#include "StrategyContractDAO.h"
#include "MySqlConnectionManager.h"
#include "SysParamsDAO.h"
#include <ctime>

 ////////////////////////////////////////////////////////////////////////
 // Name:       StrategyConstractDAO::FindStrategyContractByKey(const ContractKey& contractID)
 // Purpose:    Implementation of StrategyConstractDAO::FindStrategyContractByKey()
 // Parameters:
 // - contractID
 // Return:     std::shared_ptr<std::vector<StrategyContractDO>>
 ////////////////////////////////////////////////////////////////////////

VectorDO_Ptr<StrategyContractDO_Ptr> StrategyContractDAO::LoadStrategyContractByProductType(int productType)
{
	static const std::string sql_findstrategy(
		"SELECT exchange_symbol, contract_symbol, underlying_symbol, tick_size, multiplier, "
		"strategy_symbol, contract_type, strikeprice, expiration, accountid, "
		"product_type, bid_allowed, ask_allowed, portfolio_symbol, underlying_exchange, underlying_contract "
		"FROM vw_strategy_contract_info "
		"WHERE product_type = ?");

	auto ret = std::make_shared<VectorDO<StrategyContractDO_Ptr>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		const int maxLimitOrder = 400;
		auto map_ptr = SysParamsDAO::FindSysParams("LIMITORDER.MAXCOUNT%");

		autofillmap<UserStrategyName, autofillmap<ContractKey, StrategyPricingContract_Ptr>> pmPricingContractMap;
		RetrievePricingContractsByProductType(productType, PM, pmPricingContractMap);

		autofillmap<UserStrategyName, autofillmap<ContractKey, StrategyPricingContract_Ptr>> ivmPricingContractMap;
		RetrievePricingContractsByProductType(productType, IVM, ivmPricingContractMap);

		autofillmap<UserStrategyName, autofillmap<ContractKey, StrategyPricingContract_Ptr>> vmPricingContractMap;
		RetrievePricingContractsByProductType(productType, VM, vmPricingContractMap);

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
			std::string portfolio;
			if (!rs->isNull(14)) portfolio = rs->getString(14);

			auto exchangeID = rs->getString(1);
			auto stcdo_ptr = std::make_shared<StrategyContractDO>(exchangeID, rs->getString(2), rs->getString(10), portfolio);

			auto it = map_ptr->find("LIMITORDER.MAXCOUNT." + exchangeID);
			stcdo_ptr->AutoOrderSettings.MaxLimitOrder = it == map_ptr->end() ? maxLimitOrder : std::stoi(it->second, nullptr, 0);

			stcdo_ptr->TradingDay.Year = pTM->tm_year + 1900;
			stcdo_ptr->TradingDay.Month = pTM->tm_mon + 1;
			stcdo_ptr->TradingDay.Day = pTM->tm_mday;
			stcdo_ptr->Underlying = rs->getString(3);
			stcdo_ptr->TickSize = rs->getDouble(4);
			stcdo_ptr->Multiplier = rs->getDouble(5);
			stcdo_ptr->StrategyName = rs->getString(6);
			stcdo_ptr->ContractType = (ContractType)rs->getInt(7);
			stcdo_ptr->StrikePrice = rs->getDouble(8);
			if (!rs->isNull(9))
			{
				std::string strDate = rs->getString(9);
				stcdo_ptr->Expiration = DateType(strDate);
			}
			
			stcdo_ptr->ProductType = (ProductType)rs->getInt(11);
			stcdo_ptr->BidEnabled = rs->getBoolean(12);
			stcdo_ptr->AskEnabled = rs->getBoolean(13);

			if (!rs->isNull(15) && !rs->isNull(16))
			{
				stcdo_ptr->BaseContract = std::make_shared<ContractKey>(rs->getString(15), rs->getString(16));
			}

			auto userStrategySymb = UserStrategyName(stcdo_ptr->UserID(), stcdo_ptr->StrategyName);
			auto& modelMap = strategyDOMap.getorfill(userStrategySymb);

			std::string modelInstance;
			
			if (modelMap.tryfind(PM, modelInstance))
				stcdo_ptr->PricingModel = std::make_shared<ModelParamsDO>(modelInstance, "", stcdo_ptr->UserID());
			if (auto pMap = pmPricingContractMap.tryfind(userStrategySymb))
				if(auto pContractsPtr = pMap->tryfind(*stcdo_ptr))
					stcdo_ptr->PricingContracts = *pContractsPtr;


			if (modelMap.tryfind(IVM, modelInstance))
				stcdo_ptr->IVModel = std::make_shared<ModelParamsDO>(modelInstance, "", stcdo_ptr->UserID());
			if (auto pMap = ivmPricingContractMap.tryfind(userStrategySymb))
				if (auto pContractsPtr = pMap->tryfind(*stcdo_ptr))
					stcdo_ptr->IVMContracts = *pContractsPtr;

			if (modelMap.tryfind(VM, modelInstance))
				stcdo_ptr->VolModel = std::make_shared<ModelParamsDO>(modelInstance, "", stcdo_ptr->UserID());
			if (auto pMap = vmPricingContractMap.tryfind(userStrategySymb))
				if (auto pContractsPtr = pMap->tryfind(*stcdo_ptr))
					stcdo_ptr->VolContracts = *pContractsPtr;

			ret->push_back(stcdo_ptr);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}


void StrategyContractDAO::RetrievePricingContractsByProductType(int productType, const std::string& modelaim,
	autofillmap<UserStrategyName, autofillmap<ContractKey, StrategyPricingContract_Ptr>>& pricingContractMap)
{
	static const std::string sql_retrieve_pricingcontracts(
		"SELECT accountid, strategy_symbol, strategy_exchange, strategy_contract, "
		"pricing_exchange, pricing_contract, underlying_symbol, weight, adjust "
		"FROM vw_pricing_contract_property "
		"WHERE strategy_product_type = ? and modelaim = ?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_retrieve_pricingcontracts));
		prestmt->setInt(1, productType);
		prestmt->setString(2, modelaim);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			auto& userStrategyName = UserStrategyName(rs->getString(1), rs->getString(2));
			auto& pricingContracts = pricingContractMap.getorfill(userStrategyName);
			auto& contractKey = ContractKey(rs->getString(3), rs->getString(4));
			auto pStrategyPricingContractPtr = pricingContracts.tryfind(contractKey);
			if (!pStrategyPricingContractPtr)
			{
				auto pair = pricingContracts.emplace(contractKey, std::make_shared<StrategyPricingContract>());
				pStrategyPricingContractPtr = &pair.first->second;
			}
			PricingContract cp(rs->getString(5), rs->getString(6), rs->getString(7), rs->getDouble(8), rs->getDouble(9));
			(*pStrategyPricingContractPtr)->PricingContracts.push_back(std::move(cp));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
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
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
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
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

void StrategyContractDAO::UpdateStrategy(const StrategyContractDO & strategyDO)
{
	static const std::string sql_savepStrategy(
		"INSERT INTO strategy (accountid, trading_exchange_symbol, trading_contract_symbol, strategy_symbol, bid_allowed, ask_allowed) "
		"VALUES(?, ?, ?, ?, ?, ?) "
		"ON DUPLICATE KEY UPDATE bid_allowed = ?, ask_allowed = ? ");

	ModelParamsDO_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(session->getConnection()->prepareStatement(sql_savepStrategy));
		prestmt->setString(1, strategyDO.UserID());
		prestmt->setString(2, strategyDO.ExchangeID());
		prestmt->setString(3, strategyDO.InstrumentID());
		prestmt->setString(4, strategyDO.StrategyName);
		prestmt->setBoolean(5, strategyDO.BidEnabled);
		prestmt->setBoolean(6, strategyDO.AskEnabled);
		prestmt->setBoolean(7, strategyDO.BidEnabled);
		prestmt->setBoolean(8, strategyDO.AskEnabled);

		prestmt->executeUpdate();
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

void StrategyContractDAO::UpdateStrategyModel(const StrategyContractDO & strategyDO)
{
	static const std::string sql_savepStrategy(
		"CALL StrategyModel_Update(?,?,?,?,?)");

	ModelParamsDO_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(session->getConnection()->prepareStatement(sql_savepStrategy));
		prestmt->setString(1, strategyDO.UserID());
		prestmt->setString(2, strategyDO.StrategyName);
		strategyDO.PricingModel ? prestmt->setString(3, strategyDO.PricingModel->InstanceName) : prestmt->setNull(3, sql::DataType::VARCHAR);
		strategyDO.IVModel ? prestmt->setString(4, strategyDO.IVModel->InstanceName) : prestmt->setNull(4, sql::DataType::VARCHAR);
		strategyDO.VolModel ? prestmt->setString(5, strategyDO.VolModel->InstanceName) : prestmt->setNull(5, sql::DataType::VARCHAR);

		prestmt->executeUpdate();
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

void StrategyContractDAO::UpdatePricingContract(const UserContractKey& userContractKey, const std::string& strategySymb,
	const std::string& modelAim, const StrategyPricingContract& sto)
{
	static const std::string sql_delricingcontract(
		"DELETE FROM strategy_pricing_contract "
		"WHERE accountid=? AND strategy_exchange=? AND strategy_contract=? AND strategy_symbol=? AND modelaim=?");

	static const std::string sql_savepricingcontract(
		"INSERT INTO strategy_pricing_contract (accountid, strategy_exchange, strategy_contract, strategy_symbol, modelaim, "
		"pricing_exchange, pricing_contract, weight, adjust) VALUES (?,?,?,?,?,?,?,?,?) "
		"ON DUPLICATE KEY UPDATE weight = ?, adjust = ?");

	ModelParamsDO_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		auto conn = session->getConnection();
		AutoClosePreparedStmt_Ptr delstmt(conn->prepareStatement(sql_delricingcontract));
		delstmt->setString(1, userContractKey.UserID());
		delstmt->setString(2, userContractKey.ExchangeID());
		delstmt->setString(3, userContractKey.InstrumentID());
		delstmt->setString(4, strategySymb);
		delstmt->setString(5, modelAim);
		delstmt->executeUpdate();

		AutoClosePreparedStmt_Ptr prestmt(conn->prepareStatement(sql_savepricingcontract));
		prestmt->setString(1, userContractKey.UserID());
		prestmt->setString(2, userContractKey.ExchangeID());
		prestmt->setString(3, userContractKey.InstrumentID());
		prestmt->setString(4, strategySymb);
		prestmt->setString(5, modelAim);
		for (auto& contract : sto.PricingContracts)
		{
			prestmt->setString(6, contract.ExchangeID());
			prestmt->setString(7, contract.InstrumentID());
			prestmt->setDouble(8, contract.Weight);
			prestmt->setDouble(9, contract.Adjust);
			prestmt->setDouble(10, contract.Weight);
			prestmt->setDouble(11, contract.Adjust);
			prestmt->executeUpdate();
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

VectorDO_Ptr<ContractParamDO> StrategyContractDAO::RetrieveContractParamByUser(const std::string& userid, int productType)
{
	static const std::string sql_findcontractparam(
		"SELECT distinct pricing_exchange, pricing_contract, tick_size, multiplier "
		"FROM vw_pricing_contract_property "
		"WHERE accountid = ? and strategy_product_type = ?");

	auto ret = std::make_shared<VectorDO<ContractParamDO>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontractparam));
		prestmt->setString(1, userid);
		prestmt->setInt(2, productType);

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
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}