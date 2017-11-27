#if !defined(__message_MessageWorkerProcessor_h)
#define __message_MessageWorkerProcessor_h

#include "UserInfo.h"
#include "../dataobject/dataobjectbase.h"
#include <future>
#include "SessionContainer.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageWorkerProcessor
{
public:
	MessageWorkerProcessor();
	~MessageWorkerProcessor();

	virtual int LoginSystemUser(void) = 0;
	virtual void DispatchUserMessage(int msgId, int serialId, const std::string& userId, const dataobj_ptr& dataobj_ptr) = 0;

	virtual bool HasLogged(void);
	virtual bool ConnectedToServer(void);
	virtual const IUserInfo& GetSystemUser();

	virtual void RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr);

protected:
	bool _isLogged = false;
	bool _isConnected = false;
	UserInfo _systemUser;
	volatile bool _closing = false;
	SessionContainer_Ptr<std::string> _userSessionCtn_Ptr;
};

#endif