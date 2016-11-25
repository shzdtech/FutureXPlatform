#include "VersionDAO.h"
#include "MySqlConnectionManager.h"

bool VersionDAO::UpsertVersion(const std::string& item, const std::string& version)
{
	bool ret = false;
	static const std::string sql_updateversion(
		"INSERT INTO version (item,ver) VALUES(?,?) "
		"ON DUPLICATE KEY UPDATE ver=?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_updateversion));

		prestmt->setString(1, item);
		prestmt->setString(2, version);
		prestmt->setString(3, version);

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

bool VersionDAO::GetVersion(const std::string& item, std::string& version)
{
	bool ret = false;
	static const std::string sql_updateversion(
		"SELECT ver FROM version where item=?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_updateversion));
		prestmt->setString(1, item);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		if (rs->next())
		{
			version = rs->getString(1);
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