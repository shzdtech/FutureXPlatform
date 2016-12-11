/***********************************************************************
 * Module:  MessageProcessor.h
 * Author:  milk
 * Modified: 2014年10月6日 13:51:19
 * Purpose: Declaration of the class MessageProcessor
 ***********************************************************************/

#if !defined(__message_MessageProcessor_h)
#define __message_MessageProcessor_h

#include "../dataobject/data_buffer.h"
#include "IMessageProcessor.h"
#include "IMessageServiceLocator.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageProcessor : 
	public IMessageProcessor,
	public std::enable_shared_from_this<MessageProcessor>
{
public:
	MessageProcessor();
	~MessageProcessor();
	IMessageSession_Ptr& getMessageSession(void);
	void setMessageSession(const IMessageSession_Ptr& msgSession_ptr);
	IMessageServiceLocator_Ptr& getServiceLocator(void);
	void setServiceLocator(const IMessageServiceLocator_Ptr& svc_locator_ptr);

	virtual void setServerContext(IServerContext* serverCtx);
	virtual IServerContext* getServerContext(void);

protected:
	IMessageSession_Ptr _msgsession_ptr;
	IMessageServiceLocator_Ptr _svc_locator_ptr;
	IServerContext* _serverCtx;

	bool OnSessionClosing(void);
private:
};

#endif