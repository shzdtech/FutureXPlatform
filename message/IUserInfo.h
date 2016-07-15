/***********************************************************************
 * Module:  IUserInfo.h
 * Author:  milk
 * Modified: 2014年10月5日 18:52:26
 * Purpose: Declaration of the class IUserInfo
 ***********************************************************************/

#if !defined(__message_IUserInfo_h)
#define __message_IUserInfo_h

#include "../common/typedefs.h"
#include <ctime>

class IUserInfo
{
public:
	virtual attribute_ptr getAttribute(const std::string& key) const = 0;
	virtual void setAttribute(const std::string& key, attribute_ptr value) = 0;

	virtual const std::string& getBrokerId(void) const = 0;
	virtual void setBrokerId(const std::string& newBrokerId) = 0;

	virtual const std::string& getUserId(void) const = 0;
	virtual void setUserId(const std::string& newUserId) = 0;

	virtual void setInvestorId(const std::string& newUserId) = 0;
	virtual const std::string& getInvestorId(void) const = 0;

	virtual const std::string& getPassword(void) const = 0;
	virtual void setPassword(const std::string& newPassword) = 0;

	virtual void setName(const std::string& newName) = 0;
	virtual const std::string& getName(void) const = 0;

	virtual void setExtInfo(std::shared_ptr<void> extInfo) = 0;
	virtual void* getExtInfo(void) const = 0;

	virtual const std::string& getAuthorizedKey(void) const = 0;
	virtual void setAuthorizedKey(const std::string& newAuthKey) = 0;

	virtual std::time_t getLoginTime(void) const = 0;
	virtual void setLoginTime(const std::time_t time) = 0;

	virtual int getRole(void) const = 0;
	virtual void setRole(const int role) = 0;

	virtual int getPermission(void) const = 0;
	virtual void setPermission(const int permission) = 0;

	virtual unsigned long getNextSeq(void) = 0;
	virtual void setInitSeq(const unsigned long initVal) = 0;

	virtual int getSessionId(void) const = 0;
	virtual void setSessionId(const int sessionId) = 0;

	virtual int getFrontId(void) const = 0;
	virtual void setFrontId(const int sessionId) = 0;

	virtual const std::string& getServer(void) const = 0;
	virtual void setServer(const std::string& newServer) = 0;

protected:
private:

};

enum PermissionType
{
	ALLOW_TRADING = 1
};

enum UserRoleType
{
	ROLE_UNSPECIFIED = 0,
	ROLE_CLIENT = 1,
	ROLE_TRADINGDESK = 2,
	ROLE_ADMIN = 1024,
};

typedef std::shared_ptr<IUserInfo> IUserInfo_Ptr;
typedef std::map<std::string, IUserInfo_Ptr> IUserInfoPtrMap;
typedef std::shared_ptr<IUserInfoPtrMap> UserInfoCache;

#endif