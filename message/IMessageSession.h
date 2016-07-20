/***********************************************************************
 * Module:  IMessageSession.h
 * Author:  milk
 * Modified: 2014年10月6日 9:53:14
 * Purpose: Declaration of the class IMessageSession
 ***********************************************************************/

#if !defined(__message_IMessageSession_h)
#define __message_IMessageSession_h

#include "ISession.h"
#include "MessageInterfaceDeclare.h"

class IMessageSession : public ISession
{
public:
	virtual bool Start(void) = 0;
	virtual bool Close(void) = 0;
	virtual void RegistProcessor(IMessageProcessor_Ptr msgprocessor) = 0;
	virtual void setTimeout(long seconds) = 0;
	virtual void addListener(IMessageSessionEvent_WkPtr listener) = 0;
	virtual void removeListener(IMessageSessionEvent_WkPtr listener) = 0;

protected:
private:

};

class IMessageSessionEvent
{
public:
	virtual void OnSessionClosing(IMessageSession_Ptr msgSessionPtr) = 0;
};


#endif