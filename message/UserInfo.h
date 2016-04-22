/***********************************************************************
 * Module:  UserInfo.h
 * Author:  milk
 * Modified: 2014年10月1日 1:42:13
 * Purpose: Declaration of the class UserInfo
 ***********************************************************************/

#if !defined(__message_UserInfo_h)
#define __message_UserInfo_h

#include "IUserInfo.h"
#include <map>
#include <atomic>
#include "message_exp.h"
#include "../dataobject/data_commondef.h"

class MESSAGE_CLASS_EXPORT UserInfo : public IUserInfo
{
public:
	virtual Attrib_Ptr getAttribute(const std::string& key) const;
	virtual void setAttribute(const std::string& key, Attrib_Ptr value);
	virtual const std::string& getBrokerId(void) const;
	virtual void setBrokerId(const std::string& newBrokerId);
	virtual const std::string& getUserId(void) const;
	virtual void setUserId(const std::string& newUserId);
	virtual void setInvestorId(const std::string& newUserId);
	virtual const std::string& getInvestorId(void) const;
	virtual const std::string& getPassword(void) const;
	virtual void setPassword(const std::string& newPassword);
	virtual const std::string& getContactNum(void) const;
	virtual void setContactNum(const std::string& newContactNum);
	virtual const std::string& getName(void) const;
	virtual void setName(const std::string& newName);
	virtual const std::string& getCompany(void) const;
	virtual void setCompany(const std::string& newCompany);
	virtual const std::string& getEmail(void) const;
	virtual void setEmail(const std::string& newEmail);
	virtual int getRole(void) const;
	virtual void setRole(const int role);
	virtual int getPermission(void) const;
	virtual void setPermission(const int permission);
	virtual unsigned long getNextSeq(void);
	virtual void setInitSeq(const unsigned long initVal);
	virtual int getSessionId(void) const;
	virtual void setSessionId(const int sessionId);
	virtual int getFrontId(void) const;
	virtual void setFrontId(const int sessionId);
	virtual const std::string& getServer(void) const;
	virtual void setServer(const std::string& newServer);

protected:
private:
	std::map<std::string, Attrib_Ptr> _attributes;
	std::string _brokerId;
	std::string _userId;
	std::string _investorId;
	std::string _password;
	std::string _name;
	std::string _company;
	std::string _email;
	std::string _contactnum;
	std::string _server;
	int _role;
	int _permission;
	int _sessionId = 0;
	int _frontId = 0;
	std::atomic_ulong _seqGen;
};
#endif