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
#include "IRawAPI.h"
#include "SessionManager.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageProcessor : 
	public IMessageProcessor,
	public std::enable_shared_from_this < MessageProcessor >
{
public:
	MessageProcessor();
	~MessageProcessor();
	IMessageSession* getSession(void);
	void setSession(IMessageSession* msgSession);
	IMessageServiceLocator_Ptr getServiceLocator(void);
	void setServiceLocator(IMessageServiceLocator_Ptr svc_locator_ptr);

	virtual IRawAPI* getRawAPI(void) = 0;
	virtual int OnRecvMsg(const uint msgId, const data_buffer& msg) = 0;
	virtual int OnResponse(const uint msgId, ParamVector& rawRespParams) = 0;

protected:
	IMessageSession* _pMsgSession;
	IMessageServiceLocator_Ptr _svc_locator_ptr;

	bool OnSessionClosing(void){ return true; };
private:
};

#endif