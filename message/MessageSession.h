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
#include "IUserInfo.h"

#include <list>
#include "message_exp.h"


class MESSAGE_CLASS_EXPORT MessageSession : public IMessageSession, 
	public std::enable_shared_from_this<MessageSession>
{
public:
	MessageSession();
	~MessageSession();

	uint64_t Id();
	void RegistProcessor(const IMessageProcessor_Ptr& msgprocessor_ptr);
	bool Start(void) { return false; }
	bool Close(void);
	virtual int WriteMessage(const uint msgId, const data_buffer& msg) { return 0; }
	virtual int WriteMessage(const data_buffer& msg) { return 0; }
	void setTimeout(long seconds);
	IMessageContext_Ptr getContext(void);
	time_t getLoginTimeStamp(void);
	void setLoginTimeStamp(time_t tm);
	IUserInfo_Ptr getUserInfo(void);
	IProcessorBase_Ptr getProcessor(void);

	void addListener(const IMessageSessionEvent_WkPtr& listener);
	void removeListener(const IMessageSessionEvent_WkPtr& listener);

protected:
	uint64_t _id;
	IMessageContext_Ptr _context_ptr;
	IMessageProcessor_Ptr _messageProcessor_ptr;
	std::list<IMessageSessionEvent_WkPtr> _sessionEventList;
	IUserInfo_Ptr _userInfo_ptr;
	long _timeout;
	time_t _loginTimeStamp;
	bool _closed;

private:

};

#endif