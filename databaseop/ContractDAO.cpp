/***********************************************************************
 * Module:  ContractDAO.cpp
 * Author:  milk
 * Modified: 2015年8月8日 22:22:10
 * Purpose: Implementation of the class ContractDAO
 ***********************************************************************/

#include "ContractDAO.h"
#include "ConnectionHelper.h"


////////////////////////////////////////////////////////////////////////
// Name:       ContractDAO::FindBaseContractByCompany(const std::string& company)
// Purpose:    Implementation of ContractDAO::FindBaseContractByCompany()
// Parameters:
// - company
// Return:     std::shared_ptr<std::vector<ContractKey>>
////////////////////////////////////////////////////////////////////////
static const std::string sql_findcontract(
	"SELECT exchange_symbol, contract_symbol "
	"FROM vw_pricing_contract "
	"WHERE client_symbol = ?");

VectorDO_Ptr<ContractKey> ContractDAO::FindContractByCompany(const std::string& company)
{
	auto ret = std::make_shared<VectorDO<ContractKey>>();

	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontract));
		prestmt->setString(1, company);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ContractKey ck(rs->getString(1), rs->getString(2));
			ret->push_back(std::move(ck));
		}
	}
	catch (std::exception& ex)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << ex.what();
	}

	return ret;
}


static const std::string sql_findallcontract(
	"SELECT a.exchange_symbol, a.contract_symbol, b.tick_size, b.multiplier FROM contract a "
	"JOIN underlying b ON b.exchange_symbol = a.exchange_symbol AND b.underlying_symbol = a.underlying_symbol");

////////////////////////////////////////////////////////////////////////
// Name:       ContractDAO::FindBaseContractByCompany(const std::string& company)
// Purpose:    Implementation of ContractDAO::FindBaseContractByCompany()
// Parameters:
// - company
// Return:     std::shared_ptr<std::vector<ContractKey>>
////////////////////////////////////////////////////////////////////////

std::shared_ptr<ContractDOMap> ContractDAO::FindAllContract(void)
{
	auto ret = std::make_shared<ContractDOMap>();

	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findallcontract));

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ContractDO cdo(rs->getString(1), rs->getString(2));
			cdo.TickSize = rs->getDouble(3);
			cdo.Multiplier = rs->getDouble(4);
			ret->emplace(cdo, std::move(cdo));
		}
	}
	catch (std::exception& ex)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << ex.what();
	}

	return ret;
}


static const std::string sql_findcontractparam(
	"SELECT distinct exchange_symbol, contract_symbol, tick_size, multiplier "
	"FROM vw_pricing_contract_property "
	"WHERE client_account = ?");

VectorDO_Ptr<ContractDO> ContractDAO::FindContractParamByUser(const std::string& userID)
{
	auto ret = std::make_shared<VectorDO<ContractDO>>();

	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findcontractparam));
		prestmt->setString(1, userID);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ContractDO cdo(rs->getString(1), rs->getString(2));
			cdo.TickSize = rs->getDouble(3);
			cdo.Multiplier = rs->getDouble(4);
			ret->push_back(std::move(cdo));
		}
	}
	catch (std::exception& ex)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << ex.what();
	}

	return ret;
}