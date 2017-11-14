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
	virtual IMessageProcessor_Ptr& getWorkerProcessor(void) = 0;
	virtual void setSubTypeWorkerPtr(void* pProcess) = 0;
	virtual void* getSubTypeWorkerPtr(void) = 0;
	virtual void setAbstractSubTypeWorkerPtr(void* pProcess) = 0;
	virtual void* getAbstractSubTypeWorkerPtr(void) = 0;
	virtual bool getConfigVal(const std::string& cfgKey, std::string& cfgVal) = 0;
	virtual void setConfigVal(const std::string& cfgKey, const std::string& cfgVal) = 0;
	virtual int getServerType(void) = 0;
	virtual void setServerType(int type) = 0;
	virtual const std::string& getServerUri(void) = 0;
	virtual void Reset(void) = 0;

protected:
private:

};

typedef std::shared_ptr<IServerContext> IServerContext_Ptr;

#endif