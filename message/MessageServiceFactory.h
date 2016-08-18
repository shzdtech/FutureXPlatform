/***********************************************************************
 * Module:  MessageServiceFactory.h
 * Author:  milk
 * Modified: 2014年10月6日 23:58:25
 * Purpose: Declaration of the class MessageServiceFactory
 ***********************************************************************/

#if !defined(__message_MessageServiceFactory_h)
#define __message_MessageServiceFactory_h

#include "IMessageServiceFactory.h"
#include "IMessageHandler.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageServiceFactory : public IMessageServiceFactory
{
public:
	virtual void SetServerContext(IServerContext* serverCtx);
	virtual std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(IServerContext* serverCtx);
	virtual std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(IServerContext* serverCtx);
	virtual IMessageProcessor_Ptr CreateWorkerProcessor(IServerContext* serverCtx);
	virtual IMessageProcessor_Ptr CreateMessageProcessor(IServerContext* serverCtx);
	virtual bool LoadParams(const std::string& configFile, const std::string& section);

protected:
	std::map<std::string, std::string> _configMap;
	IServerContext* _serverCtx;

private:

};

#endif