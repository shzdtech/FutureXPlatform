/***********************************************************************
 * Module:  UserInfoDAO.cpp
 * Author:  milk
 * Modified: 2015年8月1日 21:28:29
 * Purpose: Implementation of the class UserInfoDAO
 ***********************************************************************/

#include "../message/IUserInfo.h"
#include "UserInfoDAO.h"
#include "ConnectionHelper.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       UserInfoDAO::FindUser(const std::string& userId, const std::string& password)
 // Purpose:    Implementation of UserInfoDAO::FindUser()
 // Parameters:
 // - userId
 // - password
 // Return:     std::shared_ptr<UserInfoDO>
 ////////////////////////////////////////////////////////////////////////

int UserInfoDAO::InsertUser(const UserInfoDO & userDO)
{
	int ret = -1;

	static const std::string sql_proc_newuser("CALL User_New(?,?,?,?,?,?,?,?,?,?,?)");

	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_newuser));

		prestmt->setString(1, userDO.UserId);
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
		LOG(ERROR) << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw BizError(DB_ERROR, sqlEx.getSQLStateCStr(), sqlEx.getErrorCode());
	}

	return ret;
}

std::shared_ptr<UserInfoDO> UserInfoDAO::FindUser(const std::string& userId)
{
	static const std::string sql_finduser(
		"SELECT account_password,firstname,lastname,client_symbol,is_trading_allowed,"
		"email,gender,contactnum,address,zipcode,roletype "
		"FROM vw_client_detail WHERE account_username = ?");

	std::shared_ptr<UserInfoDO> ret;

	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_finduser));
		prestmt->setString(1, userId);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			auto userDO = new UserInfoDO;
			ret.reset(userDO);
			userDO->UserId = userId;
			userDO->Password = rs->getString(1);
			userDO->FirstName = rs->getString(2);
			userDO->LastName = rs->getString(3);
			userDO->Company = rs->getString(4);
			bool allowTrading = rs->getBoolean(5);
			userDO->Permission = allowTrading ? ALLOW_TRADING : 0;
			userDO->Email = rs->getString(6);
			userDO->Gender = (GenderType)rs->getInt(7);
			userDO->ContactNum = rs->getString(8);
			userDO->Address = rs->getString(9);
			userDO->ZipCode = rs->getString(10);
			userDO->Role = rs->getInt(11);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw BizError(DB_ERROR, sqlEx.getSQLStateCStr(), sqlEx.getErrorCode());
	}

	return ret;
}

VectorDO_Ptr<UserInfoDO> UserInfoDAO::FindAllUserByRole(int role)
{
	static const std::string sql_findalluser(
		"SELECT account_username,firstname,lastname,client_symbol,is_trading_allowed,"
		"email,gender,contactnum,address,zipcode "
		"FROM vw_client_detail WHERE roletype = ?");

	auto ret = std::make_shared<VectorDO<UserInfoDO>>();
	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
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
			userDO.FirstName = rs->getString(2);
			userDO.LastName = rs->getString(3);
			userDO.Company = rs->getString(4);
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
		LOG(ERROR) << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw BizError(DB_ERROR, sqlEx.getSQLStateCStr(), sqlEx.getErrorCode());
	}

	return ret;
}