/***********************************************************************
 * Module:  SysParamsDAO.cpp
 * Author:  milk
 * Modified: 2015年8月3日 0:00:08
 * Purpose: Implementation of the class SysParamsDAO
 ***********************************************************************/

#include "SysParamsDAO.h"
#include "MySqlConnectionManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       SysParamsDAO::FindSysParams(const std::string& key)
// Purpose:    Implementation of SysParamsDAO::FindSysParams()
// Parameters:
// - key
// Return:     std::shared_ptr<std::map<std::string,std::string>>
////////////////////////////////////////////////////////////////////////

std::shared_ptr<std::map<std::string, std::string>> SysParamsDAO::FindSysParams(const std::string& key)
{
	const static std::string sql_findparams(
		"SELECT keycode, val FROM sys_param WHERE keycode LIKE ?");

	std::map<std::string, std::string> sysparams;
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findparams));
		prestmt->setString(1, key);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
			sysparams[rs->getString(1)] = rs->getString(2);
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	auto ret = std::make_shared<std::map<std::string, std::string>>(std::move(sysparams));

	return ret;
}

bool SysParamsDAO::FindSysParamValue(const std::string & key, std::string & value)
{
	bool ret = false;

	const static std::string sql_findparams(
		"SELECT val FROM sys_param WHERE keycode = ?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findparams));
		prestmt->setString(1, key);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
			value = rs->getString(1);

		ret = true;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

bool SysParamsDAO::UpsertSysParamValue(const std::string & key, std::string & value)
{
	bool ret = false;

	const static std::string sql_findparams(
		"INSERT INTO sys_param (keycode, val) VALUES (?,?) ON DUPLICATE KEY UPDATE val = ?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findparams));
		prestmt->setString(1, key);
		prestmt->setString(2, value);
		prestmt->setString(3, value);
		
		prestmt->executeUpdate();

		ret = true;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}
