/***********************************************************************
 * Module:  IMessageProcessor.h
 * Author:  milk
 * Modified: 2014年10月6日 12:49:50
 * Purpose: Declaration of the class IMessageProcessor
 ***********************************************************************/

#if !defined(__message_IMessageProcessor_h)
#define __message_IMessageProcessor_h

#include "IProcessorBase.h"
#include "IMessageSession.h"

class IMessageProcessor : public IProcessorBase
{
public:
	virtual IMessageSession* getSession(void) = 0;
	virtual void setSession(IMessageSession* msgSession) = 0;
	virtual void setServiceLocator(IMessageServiceLocator_Ptr svc_locator_ptr) = 0;
	virtual int OnRecvMsg(const uint msgId, const data_buffer& msg) = 0;
	virtual int OnResponse(const uint msgId, ParamVector& rawRespParams) = 0;
	virtual bool OnSessionClosing(void) = 0;

protected:
private:

};

#endif