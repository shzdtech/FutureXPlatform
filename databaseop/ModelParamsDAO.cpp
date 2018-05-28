#include "ModelParamsDAO.h"
#include "MySqlConnectionManager.h"

ModelParamsDO_Ptr ModelParamsDAO::FindUserModel(const std::string& userid, const std::string& modelInstance)
{
	static const std::string sql_findstrategyparam(
		"SELECT model,modelaim,paramname,paramvalue,paramstring FROM vm_usermodel_params "
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
			ret->ModelAim = rs->getString(2);
			if (rs->isNull(5))
				ret->Params[rs->getString(3)] = rs->getDouble(4);
			else
				ret->ParamString[rs->getString(3)] = rs->getString(5);
		}

		while (rs->next())
		{
			if (rs->isNull(5))
				ret->Params[rs->getString(3)] = rs->getDouble(4);
			else
				ret->ParamString[rs->getString(3)] = rs->getString(5);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

void ModelParamsDAO::FindAllModels(const std::string& userId, autofillmap<ModelKey, ModelParamsDO_Ptr>& modelMap)
{
	static const std::string sql_findstrategyparam(
		"SELECT accountid,modelinstance,model,modelaim,paramname,paramvalue,paramstring FROM vm_usermodel_params "
		"WHERE accountid like ?");

	VectorDO_Ptr<ModelParamsDO_Ptr> ret = std::make_shared<VectorDO<ModelParamsDO_Ptr>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(session->getConnection()->prepareStatement(sql_findstrategyparam));

		prestmt->setString(1, userId.empty() ? "%" : userId);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			auto userid = rs->getString(1);
			auto instance = rs->getString(2);
			auto model = rs->getString(3);
			auto modelaim = rs->getString(4);
			auto& model_ptr = modelMap.getorfillfunc(ModelKey(instance, userid), [&instance, &model, &modelaim, &userid]
			{
				auto ret = std::make_shared<ModelParamsDO>(instance, model, userid);
				ret->ModelAim = modelaim;
				return ret;
			});

			if (rs->isNull(7))
				model_ptr->Params[rs->getString(5)] = rs->getDouble(6);
			else
				model_ptr->ParamString[rs->getString(5)] = rs->getString(7);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
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

		for (auto& pair : modelParams.Params)
		{
			prestmt->setString(4, pair.first);
			prestmt->setDouble(5, pair.second);
			prestmt->setDouble(6, pair.second);
			prestmt->executeUpdate();
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

void ModelParamsDAO::NewUserModel(const ModelParamsDO & modelParams)
{
	static const std::string sql_newmodel(
		"INSERT INTO usermodels (accountid, modelinstance, model) VALUES (?,?,?) "
		"ON DUPLICATE KEY UPDATE model = ?");

	static const std::string sql_saveparam(
		"INSERT INTO model_params(accountid, modelinstance, model, paramname, paramvalue) "
		"SELECT ?, ?, model, paramname, defaultval FROM modelparamdef WHERE model = ?");

	ModelParamsDO_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(session->getConnection()->prepareStatement(sql_newmodel));
		prestmt->setString(1, modelParams.UserID());
		prestmt->setString(2, modelParams.InstanceName);
		prestmt->setString(3, modelParams.Model);
		prestmt->setString(4, modelParams.Model);

		prestmt->executeUpdate();

		AutoClosePreparedStmt_Ptr prestmtParams(session->getConnection()->prepareStatement(sql_saveparam));
		prestmtParams->setString(1, modelParams.UserID());
		prestmtParams->setString(2, modelParams.InstanceName);
		prestmtParams->setString(3, modelParams.Model);

		prestmtParams->executeUpdate();
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}
}

ModelParamDefDO_Ptr ModelParamsDAO::FindModelParamDef(const std::string& modelName)
{
	static const std::string sql_findmodeldef(
		"SELECT paramname, defaultval, minval, maxval, stringval, step, digits, datatype, visible, enabled FROM modelparamdef "
		"WHERE model = ?");

	ModelParamDefDO_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(session->getConnection()->prepareStatement(sql_findmodeldef));

		prestmt->setString(1, modelName);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			if (!ret)
			{
				ret = std::make_shared<ModelParamDefDO>(modelName);
			}

			ModelParamDef modelParamDef;
			modelParamDef.DefaultVal = rs->getDouble(2);
			if (!rs->isNull(3)) modelParamDef.MinVal = rs->getDouble(3);
			if (!rs->isNull(4)) modelParamDef.MaxVal = rs->getDouble(4);
			if (!rs->isNull(5)) modelParamDef.StringVal = rs->getString(5);
			if (!rs->isNull(6)) modelParamDef.Step = rs->getDouble(6);
			if (!rs->isNull(7)) modelParamDef.Digits = rs->getInt(7);
			if (!rs->isNull(8)) modelParamDef.DataType = rs->getInt(8);
			if (!rs->isNull(9)) modelParamDef.Visible = rs->getBoolean(9);
			if (!rs->isNull(10)) modelParamDef.Enable = rs->getBoolean(10);

			ret->ModelDefMap.emplace(rs->getString(1), std::move(modelParamDef));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}
