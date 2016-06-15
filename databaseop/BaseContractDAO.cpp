/***********************************************************************
 * Module:  BaseContractDAO.cpp
 * Author:  milk
 * Modified: 2015年8月2日 14:24:51
 * Purpose: Implementation of the class BaseContractDAO
 ***********************************************************************/

#include "BaseContractDAO.h"
#include "MySqlConnectionManager.h"
#include "SqlTemplate.h"

////////////////////////////////////////////////////////////////////////
// Name:       BaseContractDAO::FindBaseContractByParentID(ContractKey& contractID)
// Purpose:    Implementation of BaseContractDAO::FindBaseContractByParentID()
// Parameters:
// - company
// Return:     std::shard_ptr<std::vector<ContractKey>>
////////////////////////////////////////////////////////////////////////

VectorDO_Ptr<ContractKey> BaseContractDAO::FindBaseContractByParentID(ContractKey& contractID)
{
	static const std::string sql_findbasecontract(
		"SELECT base_exchange_symbol, base_contract_symbol FROM contract_base_contract a "
		"JOIN contract b ON a.contract_symbol = b.contract_symbol AND a.exchange_symbol = b.exchange_symbol "
		"WHERE b.exchange_symbol = ? AND b.contract_symbol = ?");

	auto ret = std::make_shared<VectorDO<ContractKey>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findbasecontract));
		prestmt->setString(1, contractID.ExchangeID());
		prestmt->setString(2, contractID.InstrumentID());

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ContractKey ck(rs->getString(1), rs->getString(2));
			ret->push_back(std::move(ck));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw BizError(DB_ERROR, sqlEx.getSQLStateCStr(), sqlEx.getErrorCode());
	}

	return ret;
}



////////////////////////////////////////////////////////////////////////
// Name:       BaseContractDAO::FindBaseContractByCompany(const std::string& company)
// Purpose:    Implementation of BaseContractDAO::FindBaseContractByCompany()
// Parameters:
// - company
// Return:     std::shard_ptr<std::vector<ContractKey>>
////////////////////////////////////////////////////////////////////////

VectorDO_Ptr<ContractKey> BaseContractDAO::FindBaseContractByCompany(const std::string& company)
{
	static const std::string sql_findbasecontract_co(
		"SELECT DISTINCT c.base_exchange_symbol, c.base_contract_symbol FROM contract_base_contract c "
		"JOIN contract a ON c.contract_symbol = a.contract_symbol AND c.exchange_symbol = a.exchange_symbol "
		"JOIN client_trading_limits b ON a.exchange_symbol = b.exchange_symbol AND a.underlying_symbol = b.underlying_symbol "
		"WHERE b.client_symbol = ?");

	auto ret = std::make_shared<VectorDO<ContractKey>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findbasecontract_co));

		prestmt->setString(1, company);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ContractKey ck(rs->getString(1), rs->getString(2));
			ret->push_back(std::move(ck));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw BizError(DB_ERROR, sqlEx.getSQLStateCStr(), sqlEx.getErrorCode());
	}

	return ret;
}