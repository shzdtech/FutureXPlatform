/***********************************************************************
 * Module:  ContractDAO.cpp
 * Author:  milk
 * Modified: 2015年8月8日 22:22:10
 * Purpose: Implementation of the class ContractDAO
 ***********************************************************************/

#include "ContractDAO.h"
#include "MySqlConnectionManager.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       ContractDAO::FindBaseContractByCompany(const std::string& company)
 // Purpose:    Implementation of ContractDAO::FindBaseContractByCompany()
 // Parameters:
 // - company
 // Return:     std::shared_ptr<std::vector<ContractKey>>
 ////////////////////////////////////////////////////////////////////////

VectorDO_Ptr<ContractKey> ContractDAO::FindContractByClient(const std::string& clientSymbol)
{
	static const std::string sql_findcontract(
		"SELECT exchange_symbol,contract_symbol "
		"FROM vw_pricing_contract "
		"WHERE client_symbol like ?");

	auto ret = std::make_shared<VectorDO<ContractKey>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontract));
		prestmt->setString(1, clientSymbol);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ContractKey ck(rs->getString(1), rs->getString(2));
			ret->push_back(std::move(ck));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}


////////////////////////////////////////////////////////////////////////
// Name:       ContractDAO::FindBaseContractByCompany(const std::string& company)
// Purpose:    Implementation of ContractDAO::FindBaseContractByCompany()
// Parameters:
// - company
// Return:     std::shared_ptr<std::vector<ContractKey>>
////////////////////////////////////////////////////////////////////////

VectorDO_Ptr<InstrumentDO> ContractDAO::FindContractByProductType(int productType)
{
	static const std::string sql_findallcontract(
		"SELECT exchange_symbol, contract_symbol, tick_size, multiplier, underlying_symbol, expiration, underlying_exchange, underlying_contract "
		"FROM vw_contract_property "
		"where product_type = ?");

	auto ret = std::make_shared<VectorDO<InstrumentDO>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findallcontract));
		prestmt->setInt(1, productType);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			InstrumentDO cdo(rs->getString(1), rs->getString(2));
			cdo.PriceTick = rs->getDouble(3);
			cdo.VolumeMultiple = rs->getDouble(4);
			cdo.ProductID = rs->getString(5);
			if (!rs->isNull(6))	cdo.ExpireDate = rs->getString(6);

			std::string underlying_exchange;
			std::string underlying_contract;
			if (!rs->isNull(7)) underlying_exchange = rs->getString(7);
			if (!rs->isNull(8)) underlying_contract = rs->getString(8);

			cdo.UnderlyingContract = ContractKey(underlying_exchange, underlying_contract);

			cdo.ProductType = (ProductType)productType;

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



VectorDO_Ptr<ContractParamDO> ContractDAO::FindContractParamByClient(const std::string& clientSymbol)
{
	static const std::string sql_findcontractparam(
		"SELECT distinct exchange_symbol, contract_symbol, tick_size, multiplier "
		"FROM vw_pricing_contract_property "
		"WHERE client_symbol = ?");

	auto ret = std::make_shared<VectorDO<ContractParamDO>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontractparam));
		prestmt->setString(1, clientSymbol);

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