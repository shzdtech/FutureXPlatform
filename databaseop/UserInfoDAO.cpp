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

static const std::string sql_finduser(
	"SELECT a.client_symbol, a.contact_name, a.contact_number, a.contact_email, b.roletype, a.is_trading_allowed "
	"FROM client_terminal a JOIN client b ON a.client_symbol = b.client_symbol "
	"WHERE a.account_username = ? AND a.account_password = ?");

std::shared_ptr<UserInfoDO> UserInfoDAO::FindUser(const std::string& userId, const std::string& password)
{
	std::shared_ptr<UserInfoDO> ret;

	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_finduser));
		prestmt->setString(1, userId);
		prestmt->setString(2, password);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			ret.reset(new UserInfoDO);
			ret->Company = rs->getString(1);
			ret->Name = rs->getString(2);
			ret->ContactNum = rs->getString(3);
			ret->Email = rs->getString(4);
			ret->Role = rs->getInt(5);

			bool allowTrading = rs->getBoolean(5);
			ret->Permission = allowTrading ? ALLOW_TRADING : 0;

			ret->Password = password;
			ret->UserId = userId;
		}
	}
	catch (std::exception& ex)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << ex.what();
	}

	return ret;
}


static const std::string sql_findalluser(
	"SELECT a.contact_name, a.client_symbol, a.contact_number, a.contact_email, a.is_trading_allowed "
	"FROM client_terminal a JOIN CLIENT b ON a.client_symbol = b.client_symbol "
	"WHERE b.roletype = ?");

VectorDO_Ptr<UserInfoDO> UserInfoDAO::FindAllUserByRole(int role)
{
	auto ret = std::make_shared<VectorDO<UserInfoDO>>();
	auto session = ConnectionHelper::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_finduser));
		prestmt->setInt(1, role);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			UserInfoDO userDO;
			userDO.Name = rs->getString(1);
			userDO.Company = rs->getString(2);
			userDO.ContactNum = rs->getString(3);
			userDO.Email = rs->getString(4);
			userDO.Role = role;

			bool allowTrading = rs->getBoolean(5);
			userDO.Permission = allowTrading ? ALLOW_TRADING : 0;

			ret->push_back(std::move(userDO));
		}
	}
	catch (std::exception& ex)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << ex.what();
	}

	return ret;
}