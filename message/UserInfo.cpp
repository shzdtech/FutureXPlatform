/***********************************************************************
 * Module:  UserInfo.cpp
 * Author:  milk
 * Modified: 2014年10月1日 1:42:13
 * Purpose: Implementation of the class UserInfo
 ***********************************************************************/

#include "UserInfo.h"

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getAttribute(std::string key)
// Purpose:    Implementation of UserInfo::getAttribute()
// Parameters:
// - key
// Return:     attribute_ptr
////////////////////////////////////////////////////////////////////////

UserInfo::UserInfo(const IUserInfo & userInfo)
{
	setAuthorizedKey(userInfo.getAuthorizedKey());
	setBrokerId(userInfo.getBrokerId());
	setExtInfo(userInfo.getExtInfo());
	setInvestorId(userInfo.getInvestorId());
	setName(userInfo.getName());
	setPassword(userInfo.getPassword());
	setPermission(userInfo.getPermission());
	setRole(userInfo.getRole());
	setUserId(userInfo.getUserId());
	setSharedAccount(userInfo.sharedAccount());
}

attribute_ptr UserInfo::getAttribute(const std::string& key) const
{
	attribute_ptr ret;
	auto itr = _attributes.find(key);
	if (itr != _attributes.end())
		ret = itr->second;

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setAttribute(std::string& key, attribute_ptr value)
// Purpose:    Implementation of UserInfo::setAttribute()
// Parameters:
// - key
// - value
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setAttribute(const std::string& key, const attribute_ptr& value)
{
	_attributes[key] = value;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setName(const std::string& newName)
// Purpose:    Implementation of UserInfo::setName()
// Parameters:
// - newName
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setName(const std::string& newName)
{
	_name = newName;
}

void UserInfo::setExtInfo(std::shared_ptr<void> extInfo)
{
	_extInfo = extInfo;
}

std::shared_ptr<void> UserInfo::getExtInfo(void) const
{
	return _extInfo;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getName()
// Purpose:    Implementation of UserInfo::getName()
// Return:     const std::string&
////////////////////////////////////////////////////////////////////////

const std::string& UserInfo::getName(void) const
{
	return _name;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setPassword(const std::string& newPassword)
// Purpose:    Implementation of UserInfo::setPassword()
// Parameters:
// - newPassword
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setPassword(const std::string& newPassword)
{
	_password = newPassword;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getPassword()
// Purpose:    Implementation of UserInfo::getPassword()
// Return:     const std::string&
////////////////////////////////////////////////////////////////////////

const std::string& UserInfo::getPassword(void) const
{
	return _password;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setUserId(const std::string& newUserId)
// Purpose:    Implementation of UserInfo::setUserId()
// Parameters:
// - newUserId
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setUserId(const std::string& newUserId)
{
	_userId = newUserId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getUserId()
// Purpose:    Implementation of UserInfo::getUserId()
// Return:     const std::string&
////////////////////////////////////////////////////////////////////////

const std::string& UserInfo::getUserId(void) const
{
	return _userId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getInvestorId()
// Purpose:    Implementation of UserInfo::getInvestorId()
// Return:     const std::string&
////////////////////////////////////////////////////////////////////////

const std::string& UserInfo::getInvestorId(void) const
{
	return _investorId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setInvestorId(const std::string& newUserId)
// Purpose:    Implementation of UserInfo::setInvestorId()
// Parameters:
// - newUserId
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setInvestorId(const std::string& investorId)
{
	_investorId = investorId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setBrokerId(const std::string& newBrokerId)
// Purpose:    Implementation of UserInfo::setBrokerId()
// Parameters:
// - newBrokerId
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setBrokerId(const std::string& newBrokerId)
{
	_brokerId = newBrokerId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getBrokerId()
// Purpose:    Implementation of UserInfo::getBrokerId()
// Return:     const std::string&
////////////////////////////////////////////////////////////////////////

const std::string& UserInfo::getBrokerId(void) const
{
	return _brokerId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getRole()
// Purpose:    Implementation of UserInfo::getRole()
// Return:     int
////////////////////////////////////////////////////////////////////////

int UserInfo::getRole(void) const
{
	return _role;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setRole(const int role)
// Purpose:    Implementation of UserInfo::setRole()
// Parameters:
// - role
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setRole(const int role)
{
	_role = role;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getPermission()
// Purpose:    Implementation of UserInfo::getPermission()
// Return:     int
////////////////////////////////////////////////////////////////////////

int UserInfo::getPermission(void) const
{
	return _permission;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setPermission(int permission)
// Purpose:    Implementation of UserInfo::setPermission()
// Parameters:
// - permission
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setPermission(const int permission)
{
	_permission = permission;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getNextSeq()
// Purpose:    Implementation of UserInfo::getNextSeq()
// Return:     long
////////////////////////////////////////////////////////////////////////

unsigned long UserInfo::getNextSeq(void)
{
	return _seqGen++;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setInitSeq(const long initVal)
// Purpose:    Implementation of UserInfo::setInitSeq()
// Parameters:
// - initVal
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setInitSeq(const unsigned long initVal)
{
	_seqGen = initVal;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setInitSeq(const long initVal)
// Purpose:    Implementation of UserInfo::setInitSeq()
// Parameters:
// - initVal
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setSessionId(const int sessionId)
{
	_sessionId = sessionId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getNextSeq()
// Purpose:    Implementation of UserInfo::getNextSeq()
// Return:     long
////////////////////////////////////////////////////////////////////////

int UserInfo::getSessionId(void) const
{
	return _sessionId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setInitSeq(const long initVal)
// Purpose:    Implementation of UserInfo::setInitSeq()
// Parameters:
// - initVal
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setFrontId(const int frontId)
{
	_frontId = frontId;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getNextSeq()
// Purpose:    Implementation of UserInfo::getNextSeq()
// Return:     long
////////////////////////////////////////////////////////////////////////

int UserInfo::getFrontId(void) const
{
	return _frontId;
}

void UserInfo::setServer(const std::string& newServer)
{
	_server = newServer;
}

bool UserInfo::sharedAccount(void) const
{
	return _sharedAccount;
}

void UserInfo::setSharedAccount(bool sharedAccount)
{
	_sharedAccount = sharedAccount;
}

int UserInfo::getTradingDay(void) const
{
	return _tradingDay;
}

void UserInfo::setTradingDay(const int tradingDay)
{
	_tradingDay = tradingDay;
}

const std::string& UserInfo::getServer(void) const
{
	return _server;
}


const std::string & UserInfo::getAuthorizedKey(void) const
{
	return _authKey;
}

void UserInfo::setAuthorizedKey(const std::string & newAuthKey)
{
	_authKey = newAuthKey;
}

std::time_t UserInfo::getLoginTime(void) const
{
	return _loginTm;
}

void UserInfo::setLoginTime(const std::time_t time)
{
	_loginTm = time;
}