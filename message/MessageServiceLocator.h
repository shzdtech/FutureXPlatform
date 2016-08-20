/***********************************************************************
 * Module:  MessageRouter.h
 * Author:  milk
 * Modified: 2014年10月6日 13:03:58
 * Purpose: Declaration of the class MessageRouter
 ***********************************************************************/

#if !defined(__message_MessageRouter_h)
#define __message_MessageRouter_h

#include "IMessageServiceLocator.h"
#include "IMessageServiceFactory.h"
#include <string>
#include <map>
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT MessageServiceLocator : public IMessageServiceLocator
{
public:
	MessageServiceLocator(IMessageServiceFactory_Ptr msgsvc_fac, IServerContext* serverCtx);
	~MessageServiceLocator();

	IMessageHandler_Ptr FindMessageHandler(uint msgId);
	IDataSerializer_Ptr FindDataSerializer(uint msgId);
	IMessageProcessor_Ptr GetWorkerProcessor(void);
	std::map<uint, IMessageHandler_Ptr>& AllMessageHandler(void);
	std::map<uint, IDataSerializer_Ptr>& AllDataSerializer(void);

protected:
	std::map<uint, IMessageHandler_Ptr> _msghdlMap;
	std::map<uint, IDataSerializer_Ptr> _dataSerialMap;
	IMessageProcessor_WkPtr _workProcessor;

private:

};

#endif