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
#include "IContextAttribute.h"
#include "../dataobject/IDataSerializer.h"

class IMessageServiceFactory
{
public:
	virtual void SetServerContext(IContextAttribute* serverCtx) = 0;
	virtual std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(void) = 0;
	virtual std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(void) = 0;
	virtual std::map<std::string, IProcessorBase_Ptr> CreateWorkProcessor(void) = 0;
	virtual IMessageProcessor_Ptr CreateMessageProcessor(void) = 0;
	virtual bool Load(const std::string& configFile, const std::string& param) = 0;

protected:
private:

};

typedef std::shared_ptr<IMessageServiceFactory> IMessageServiceFactory_Ptr;

#endif