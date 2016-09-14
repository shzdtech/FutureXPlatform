#include "ModelParamsDAO.h"
#include "MySqlConnectionManager.h"

ModelParamsDO_Ptr ModelParamsDAO::FindUserModel(const std::string& userid, const std::string& modelInstance)
{
	static const std::string sql_findstrategyparam(
		"SELECT model,paramname,paramvalue FROM model_params "
		"WHERE accountid = ? and modelinstance = ?");

	ModelParamsDO_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(session->getConnection()->prepareStatement(sql_findstrategyparam));
		prestmt->setString(1, userid);
		prestmt->setString(2, modelInstance);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		if (rs->next())
		{
			ret = std::make_shared<ModelParamsDO>(modelInstance, rs->getString(1), userid);
			ret->Params.emplace(rs->getString(2), rs->getDouble(3));
		}

		while (rs->next())
		{
			ret->Params.emplace(rs->getString(2), rs->getDouble(3));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

void ModelParamsDAO::SaveModelParams(const ModelParamsDO & modelParams)
{
	static const std::string sql_saveparam(
		"INSERT INTO model_params (accountid,modelinstance,model,paramname,paramvalue) VALUES (?,?,?,?,?) ON DUPLICATE KEY UPDATE paramvalue = ?");

	ModelParamsDO_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(session->getConnection()->prepareStatement(sql_saveparam));
		prestmt->setString(1, modelParams.UserID());
		prestmt->setString(2, modelParams.InstanceName);
		prestmt->setString(3, modelParams.Model);
		
		for(auto& pair : modelParams.Params)
		{
			prestmt->setString(4, pair.first);
			prestmt->setDouble(5, pair.second);
			prestmt->setDouble(6, pair.second);
			prestmt->executeUpdate();
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}
