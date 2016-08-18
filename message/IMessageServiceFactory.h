/***********************************************************************
 * Module:  IMessageServiceFactory.h
 * Author:  milk
 * Modified: 2014年10月6日 13:05:34
 * Purpose: Declaration of the class IMessageServiceFactory
 ***********************************************************************/

#if !defined(__message_IMessageServiceFactory_h)
#define __message_IMessageServiceFactory_h

#include <map>
#include "IMessageProcessor.h"
#include "IMessageHandler.h"
#include "IServerContext.h"
#include "../dataserializer/IDataSerializer.h"

class IMessageServiceFactory
{
public:
	virtual void SetServerContext(IServerContext* serverCtx) = 0;
	virtual std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx) = 0;
	virtual std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(IServerContext* serverCtx) = 0;
	virtual IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx) = 0;
	virtual IMessageProcessor_Ptr CreateMessageProcessor(IServerContext* serverCtx) = 0;
	virtual bool LoadParams(const std::string& configFile, const std::string& section) = 0;

protected:
private:

};

typedef std::shared_ptr<IMessageServiceFactory> IMessageServiceFactory_Ptr;

#endif