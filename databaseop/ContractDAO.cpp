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

VectorDO_Ptr<InstrumentDO> ContractDAO::FindContractByProductType(int productType)
{
	static const std::string sql_findallcontract(
		"SELECT exchange_symbol, contract_symbol, contract_type, tick_size, multiplier, "
		"underlying_symbol, expiration, underlying_exchange, underlying_contract, strikeprice "
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
			cdo.ContractType = (ContractType)rs->getInt(3);
			cdo.PriceTick = rs->getDouble(4);
			cdo.VolumeMultiple = rs->getDouble(5);
			cdo.ProductID = rs->getString(6);
			if (!rs->isNull(7))	cdo.ExpireDate = rs->getString(7);

			std::string underlying_exchange;
			std::string underlying_contract;
			if (!rs->isNull(8)) underlying_exchange = rs->getString(8);
			if (!rs->isNull(9)) underlying_contract = rs->getString(9);

			cdo.StrikePrice = rs->getDouble(10);

			cdo.UnderlyingContract = ContractKey(underlying_exchange, underlying_contract);

			cdo.ProductType = (ProductType)productType;

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

bool ContractDAO::UpsertContracts(const std::vector<InstrumentDO>& instuments)
{
	bool ret = false;
	static const std::string sql_updatecontract(
		"INSERT INTO contractinfo (exchange_symbol,contract_symbol,name,contract_type,underlying_symbol,expiration,"
			"strikeprice,underlying_exchange,underlying_contract,product_type,lifephase) "
			"VALUES(?,?,?,?,?,?,?,?,?,?,?) "
			"ON DUPLICATE KEY UPDATE name=?, lifephase=?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	//const InstrumentDO* lastContract;
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_updatecontract));

		for (auto& contract : instuments)
		{
			//lastContract = &contract;
			if(!contract.InstrumentID().empty() && !contract.ProductID.empty());
			{
				prestmt->setString(1, contract.ExchangeID());
				prestmt->setString(2, contract.InstrumentID());
				prestmt->setString(3, contract.Name);
				prestmt->setInt(4, contract.ContractType);
				prestmt->setString(5, contract.ProductID);
				contract.ExpireDate.empty() ? prestmt->setNull(6, sql::DataType::DATE) : prestmt->setString(6, contract.ExpireDate);
				prestmt->setDouble(7, contract.StrikePrice);
				contract.UnderlyingContract.ExchangeID().empty() ? prestmt->setNull(8, sql::DataType::VARCHAR) : prestmt->setString(8, contract.UnderlyingContract.ExchangeID());
				contract.UnderlyingContract.InstrumentID().empty() ? prestmt->setNull(9, sql::DataType::VARCHAR) : prestmt->setString(9, contract.UnderlyingContract.InstrumentID());
				prestmt->setInt(10, contract.ProductType);
				prestmt->setInt(11, contract.LifePhase);
				prestmt->setString(12, contract.Name);
				prestmt->setInt(13, contract.LifePhase);

				prestmt->executeUpdate();
			}
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