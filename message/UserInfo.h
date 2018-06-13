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
	UserInfo() = default;

	UserInfo(const IUserInfo& userInfo);

	virtual attribute_ptr getAttribute(const std::string& key) const;
	virtual void setAttribute(const std::string& key, const attribute_ptr& value);

	virtual const std::string getBrokerInvestorId(void) const;

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
	virtual std::shared_ptr<void> getExtInfo(void) const;

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

	virtual bool sharedAccount(void) const;
	virtual void setSharedAccount(bool sharedAccount);

	virtual int getTradingDay(void) const;
	virtual void setTradingDay(const int tradingDay);

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
	std::time_t _loginTm = 0;
	int _role = 0;
	int _permission = 0;
	int _sessionId = 0;
	int _frontId = 0;
	int _tradingDay = 0;
	bool _sharedAccount = false;
	std::shared_ptr<void> _extInfo;
	std::atomic_ulong _seqGen;
};
#endif