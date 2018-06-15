/***********************************************************************
 * Module:  MessageSession.h
 * Author:  milk
 * Modified: 2014年10月6日 9:55:49
 * Purpose: Declaration of the class MessageSession
 ***********************************************************************/

#if !defined(__message_MessageSession_h)
#define __message_MessageSession_h

#include "IMessageSession.h"
#include "IMessageProcessor.h"
#include "ISessionManager.h"
#include "UserInfo.h"
#include "../utility/lockfree_set.h"
#include "../utility/weak_ptr_hash.h"

#include <set>
#include "message_exp.h"


class MESSAGE_CLASS_EXPORT MessageSession : public IMessageSession, 
	public std::enable_shared_from_this<MessageSession>
{
public:
	MessageSession() = default;
	MessageSession(const ISessionManager_Ptr& sessionMgr_Ptr);
	~MessageSession();

	uint64_t Id();
	void RegisterProcessor(const IMessageProcessor_Ptr& msgprocessor_ptr);
	bool Start(void) { return false; }
	bool Close(void);
	bool NotifyClosing(void);
	virtual int WriteMessage(const uint msgId, const data_buffer& msg) { return 0; }
	virtual int WriteMessage(const data_buffer& msg) { return 0; }
	void setTimeout(long seconds);
	IMessageContext_Ptr& getContext(void);
	time_t getLoginTimeStamp(void);
	void setLoginTimeStamp(time_t tm);
	void setLogout(void);
	IUserInfo& getUserInfo(void);
	IMessageProcessor_Ptr LockMessageProcessor(void);

	void setSessionManager(const ISessionManager_Ptr& sessionMgr_Ptr);
	ISessionManager_Ptr& getSessionManager(void);

	void addListener(const IMessageSessionEvent_WkPtr& listener);
	void removeListener(const IMessageSessionEvent_WkPtr& listener);

protected:
	uint64_t _id;
	IMessageContext_Ptr _context_ptr;
	IMessageProcessor_WkPtr _messageProcessor_wkptr;
	lockfree_set<IMessageSessionEvent_WkPtr,WeakPtrHash<IMessageSessionEvent>, WeakPtrEqual<IMessageSessionEvent>> _sessionHub;
	UserInfo _userInfo;
	ISessionManager_Ptr _sessionManager_ptr;
	long _timeout;
	time_t _loginTimeStamp;

private:

};

#endif