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
	virtual void SetServerContext(IContextAttribute* serverCtx);
	virtual std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(void);
	virtual std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(void);
	virtual std::map<uint, IProcessorBase_Ptr> CreateWorkProcessor(void);
	virtual IMessageProcessor_Ptr CreateMessageProcessor(void);
	virtual bool Load(const std::string& configFile, const std::string& param);

protected:
	std::map<std::string, std::string> _configMap;
	IContextAttribute* _serverCtx;

private:

};

#endif