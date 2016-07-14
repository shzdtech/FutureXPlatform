/***********************************************************************
 * Module:  UserInfoDAO.cpp
 * Author:  milk
 * Modified: 2015年8月1日 21:28:29
 * Purpose: Implementation of the class UserInfoDAO
 ***********************************************************************/

#include "../message/IUserInfo.h"
#include "UserInfoDAO.h"
#include "MySqlConnectionManager.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       UserInfoDAO::FindUser(const std::string& userId, const std::string& password)
 // Purpose:    Implementation of UserInfoDAO::FindUser()
 // Parameters:
 // - userId
 // - password
 // Return:     std::shared_ptr<UserInfoDO>
 ////////////////////////////////////////////////////////////////////////

int UserInfoDAO::InsertUser(UserInfoDO & userDO)
{
	int ret = -1;

	static const std::string sql_proc_newuser("CALL User_New(?,?,?,?,?,?,?,?,?,?,?)");

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_newuser));

		prestmt->setString(1, userDO.UserName);
		prestmt->setString(2, userDO.Password);
		prestmt->setString(3, userDO.FirstName);
		prestmt->setString(4, userDO.LastName);
		prestmt->setString(5, userDO.Company);
		prestmt->setInt(6, userDO.Gender);
		prestmt->setString(7, userDO.Email);
		prestmt->setString(8, userDO.IdentityNum);
		prestmt->setString(9, userDO.ContactNum);
		prestmt->setString(10, userDO.Address);
		prestmt->setString(11, userDO.ZipCode);

		prestmt->execute();

		ret = 0;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

std::shared_ptr<UserInfoDO> UserInfoDAO::FindUser(const std::string& userName)
{
	static const std::string sql_finduser(
		"SELECT account_id,account_password,firstname,lastname,client_symbol,is_trading_allowed,"
		"email,gender,contactnum,address,zipcode,roletype "
		"FROM vw_client_detail WHERE account_username = ?");

	std::shared_ptr<UserInfoDO> ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_finduser));
		prestmt->setString(1, userName);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			auto userDO = new UserInfoDO;
			ret.reset(userDO);
			userDO->UserId = rs->getString(1);
			userDO->UserName = userName;
			userDO->Password = rs->getString(2);
			userDO->FirstName = rs->getString(3);
			userDO->LastName = rs->getString(4);
			userDO->Company = rs->getString(5);
			bool allowTrading = rs->getBoolean(6);
			userDO->Permission = allowTrading ? ALLOW_TRADING : 0;
			userDO->Email = rs->getString(7);
			userDO->Gender = (GenderType)rs->getInt(8);
			userDO->ContactNum = rs->getString(9);
			userDO->Address = rs->getString(10);
			userDO->ZipCode = rs->getString(11);
			userDO->Role = rs->getInt(12);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

VectorDO_Ptr<UserInfoDO> UserInfoDAO::FindAllUserByRole(int role)
{
	static const std::string sql_findalluser(
		"SELECT account_id,account_username,firstname,lastname,client_symbol,is_trading_allowed,"
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
			userDO.Company = rs->getString(5);
			bool allowTrading = rs->getBoolean(6);
			userDO.Permission = allowTrading ? ALLOW_TRADING : 0;
			userDO.Email = rs->getString(7);
			userDO.Gender = (GenderType)rs->getInt(8);
			userDO.ContactNum = rs->getString(9);
			userDO.Address = rs->getString(10);
			userDO.ZipCode = rs->getString(11);
			userDO.Role = role;

			ret->push_back(std::move(userDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}