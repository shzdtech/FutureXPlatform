/***********************************************************************
 * Module:  IServerContext.h
 * Author:  milk
 * Modified: 2014年10月6日 20:13:19
 * Purpose: Declaration of the class IServerContext
 ***********************************************************************/

#if !defined(__message_IServerContext_h)
#define __message_IServerContext_h

#include "../common/typedefs.h"
#include "IMessageContext.h"
#include "MessageInterfaceDeclare.h"

class IServerContext : public IMessageContext
{
public:
	virtual void setWorkerProcessor(const IMessageProcessor_Ptr& proc_ptr) = 0;
	virtual IMessageProcessor_Ptr getWorkerProcessor(void) = 0;
	virtual void setSubTypeWorkerPtr(const shared_void_ptr& proc_ptr) = 0;
	virtual shared_void_ptr getSubTypeWorkerPtr(void) = 0;

protected:
private:

};

typedef std::shared_ptr<IServerContext> IServerContext_Ptr;

#endif