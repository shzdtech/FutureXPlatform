/***********************************************************************
 * Module:  UserInfoDAO.cpp
 * Author:  milk
 * Modified: 2015年8月1日 21:28:29
 * Purpose: Implementation of the class UserInfoDAO
 ***********************************************************************/

#include "../message/IUserInfo.h"
#include "UserInfoDAO.h"
#include "PortfolioDAO.h"
#include "MySqlConnectionManager.h"

int UserInfoDAO::InsertUser(UserInfoDO & userDO)
{
	int ret = -1;

	static const std::string sql_proc_newuser("CALL User_New(?,?,?,?,?,?,?,?,?,?)");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_newuser));

		prestmt->setString(1, userDO.UserName);
		prestmt->setString(2, userDO.Password);
		//prestmt->setString(3, userDO.Company);
		prestmt->setString(3, userDO.ContactNum);
		prestmt->setString(4, userDO.FirstName);
		prestmt->setString(5, userDO.LastName);
		prestmt->setInt(6, userDO.Gender);
		prestmt->setString(7, userDO.Email);
		prestmt->setString(8, userDO.IdentityNum);
		prestmt->setString(9, userDO.Address);
		prestmt->setString(10, userDO.ZipCode);

		prestmt->execute();

		ret = 0;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

 ////////////////////////////////////////////////////////////////////////
 // Name:       UserInfoDAO::FindUser(const std::string& userId, const std::string& password)
 // Purpose:    Implementation of UserInfoDAO::FindUser()
 // Parameters:
 // - userId
 // - password
 // Return:     std::shared_ptr<UserInfoDO>
 ////////////////////////////////////////////////////////////////////////

std::shared_ptr<UserInfoDO> UserInfoDAO::FindUser(const std::string& userName)
{
	static const std::string sql_finduser(
		"SELECT accountid,password,firstname,lastname,is_trading_allowed,"
		"email,gender,contactnum,address,zipcode,roletype "
		"FROM vw_client_detail WHERE username = ?");

	std::shared_ptr<UserInfoDO> ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_finduser));
		prestmt->setString(1, userName);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		if (rs->next())
		{
			ret = std::make_shared<UserInfoDO>();
			ret->UserId = rs->getString(1);
			ret->UserName = userName;
			ret->Password = rs->getString(2);
			ret->FirstName = rs->getString(3);
			ret->LastName = rs->getString(4);
			//ret->Company = rs->getString(5);
			bool allowTrading = rs->getBoolean(5);
			ret->Permission = allowTrading ? ALLOW_TRADING : 0;
			ret->Email = rs->getString(6);
			ret->Gender = (GenderType)rs->getInt(7);
			ret->ContactNum = rs->getString(8);
			ret->Address = rs->getString(9);
			ret->ZipCode = rs->getString(10);
			ret->Role = rs->getInt(11);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

VectorDO_Ptr<UserInfoDO> UserInfoDAO::FindAllUserByRole(int role)
{
	static const std::string sql_findalluser(
		"SELECT accountid,username,firstname,lastname,is_trading_allowed,"
		"email,gender,contactnum,address,zipcode "
		"FROM vw_client_detail WHERE roletype = ?");

	auto ret = std::make_shared<VectorDO<UserInfoDO>>();
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findalluser));
		prestmt->setInt(1, role);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			UserInfoDO userDO;
			userDO.UserId = rs->getString(1);
			userDO.UserName = rs->getString(2);
			userDO.FirstName = rs->getString(3);
			userDO.LastName = rs->getString(4);
			//userDO.Company = rs->getString(5);
			bool allowTrading = rs->getBoolean(5);
			userDO.Permission = allowTrading ? ALLOW_TRADING : 0;
			userDO.Email = rs->getString(6);
			userDO.Gender = (GenderType)rs->getInt(7);
			userDO.ContactNum = rs->getString(8);
			userDO.Address = rs->getString(9);
			userDO.ZipCode = rs->getString(10);
			userDO.Role = role;

			ret->push_back(std::move(userDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

int UserInfoDAO::ResetPassword(const std::string& userId, const std::string& password)
{
	int ret = -1;

	static const std::string sql_resetpassword("UPDATE login SET password = ? WHERE accountid = ?");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_resetpassword));

		prestmt->setString(1, password);
		prestmt->setString(2, userId);

		prestmt->executeUpdate();

		ret = 0;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}