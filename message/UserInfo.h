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
#include "../common/typedefs.h"

class MESSAGE_CLASS_EXPORT UserInfo : public IUserInfo
{
public:
	virtual attribute_ptr getAttribute(const std::string& key) const;
	virtual void setAttribute(const std::string& key, attribute_ptr value);

	virtual const std::string& getBrokerId(void) const;
	virtual void setBrokerId(const std::string& newBrokerId);

	virtual const std::string& getUserId(void) const;
	virtual void setUserId(const std::string& newUserId);

	virtual void setInvestorId(const std::string& newUserId);
	virtual const std::string& getInvestorId(void) const;

	virtual const std::string& getPassword(void) const;
	virtual void setPassword(const std::string& newPassword);

	virtual const std::string& getName(void) const;
	virtual void setName(const std::string& newName);

	virtual void setExtInfo(std::shared_ptr<void> extInfo);
	virtual void* getExtInfo(void) const;

	virtual const std::string& getAuthorizedKey(void) const;
	virtual void setAuthorizedKey(const std::string& newAuthKey);

	virtual std::time_t getLoginTime(void) const;
	virtual void setLoginTime(const std::time_t time);

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
	std::map<std::string, attribute_ptr> _attributes;
	std::string _brokerId;
	std::string _userId;
	std::string _investorId;
	std::string _password;
	std::string _name;
	std::string _server;
	std::string _authKey;
	std::time_t _loginTm;
	int _role;
	int _permission;
	int _sessionId;
	int _frontId;
	std::shared_ptr<void> _extInfo;
	std::atomic_ulong _seqGen;
};
#endif