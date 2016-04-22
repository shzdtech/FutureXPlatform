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
#include "SessionContext.h"
#include "message_exp.h"
#include "UserInfo.h"
#include <list>

class MESSAGE_CLASS_EXPORT MessageSession : public IMessageSession, public std::enable_shared_from_this < MessageSession >
{
public:
	MessageSession();
	~MessageSession();

	uint64_t Id();
	void RegistProcessor(IMessageProcessor_Ptr msgprocessor_ptr);
	bool Start(void) { return false; }
	bool Close(void);
	int WriteMessage(const uint msgId, const data_buffer& msg) { return 0; }
	int WriteMessage(const data_buffer& msg) { return 0; }
	void setTimeout(long seconds);
	ISessionContext_Ptr getContext(void);
	bool IsLogin(void);
	void setLoginStatus(bool status);
	IUserInfo_Ptr getUserInfo(void);
	IProcessorBase_Ptr getProcessor(void);
	void addListener(IMessageSessionEvent_WkPtr listener);
	void removeListener(IMessageSessionEvent_WkPtr listener);

protected:
	uint64_t _id;
	ISessionContext_Ptr _context_ptr;
	IMessageProcessor_Ptr _messageProcessor_ptr;
	std::list<IMessageSessionEvent_WkPtr> _sessionEventList;
	IUserInfo_Ptr _userInfo_ptr;
	long _timeout;
	bool _islogin;
	bool _closed;

private:

};

#endif