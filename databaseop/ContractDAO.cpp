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
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}