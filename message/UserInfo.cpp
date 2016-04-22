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
// Return:     Attrib_Ptr
////////////////////////////////////////////////////////////////////////

Attrib_Ptr UserInfo::getAttribute(const std::string& key) const
{
	Attrib_Ptr ret;
	auto itr = _attributes.find(key);
	if (itr != _attributes.end())
		ret = itr->second;

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setAttribute(std::string& key, Attrib_Ptr value)
// Purpose:    Implementation of UserInfo::setAttribute()
// Parameters:
// - key
// - value
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setAttribute(const std::string& key, Attrib_Ptr value)
{
	_attributes[key] = value;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setCompany(const std::string& newCompany)
// Purpose:    Implementation of UserInfo::setCompany()
// Parameters:
// - newCompany
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setCompany(const std::string& newCompany)
{
	_company = newCompany;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getCompany()
// Purpose:    Implementation of UserInfo::getCompany()
// Return:     const std::string&
////////////////////////////////////////////////////////////////////////

const std::string& UserInfo::getCompany(void) const
{
	return _company;
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
// Name:       UserInfo::setContactNum(const std::string& newContactNum)
// Purpose:    Implementation of UserInfo::setContactNum()
// Parameters:
// - newContactNum
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setContactNum(const std::string& newContactNum)
{
	_contactnum = newContactNum;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getContactNum()
// Purpose:    Implementation of UserInfo::getContactNum()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& UserInfo::getContactNum(void) const
{
	return _contactnum;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::getEmail()
// Purpose:    Implementation of UserInfo::getEmail()
// Return:     const std::string&
////////////////////////////////////////////////////////////////////////

const std::string& UserInfo::getEmail(void) const
{
	return _email;
}

////////////////////////////////////////////////////////////////////////
// Name:       UserInfo::setEmail(const std::string& newEmail)
// Purpose:    Implementation of UserInfo::setEmail()
// Parameters:
// - newEmail
// Return:     void
////////////////////////////////////////////////////////////////////////

void UserInfo::setEmail(const std::string& newEmail)
{
	_email = newEmail;
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

const std::string& UserInfo::getServer(void) const
{
	return _server;
}