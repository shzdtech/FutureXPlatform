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
	MessageServiceLocator(IMessageServiceFactory_Ptr msgsvc_fac);

	IMessageHandler_Ptr FindMessageHandler(uint msgId);
	IDataSerializer_Ptr FindDataSerializer(uint msgId);
	IProcessorBase_Ptr FindWorkProccessor(const std::string& processorName);
	std::map<uint, IMessageHandler_Ptr>& AllMessageHandler(void);
	std::map<uint, IDataSerializer_Ptr>& AllDataSerializer(void);
	std::map<std::string, IProcessorBase_Ptr>& AllWorkProcessor(void);

protected:
	std::map<uint, IMessageHandler_Ptr> _msghdlMap;
	std::map<uint, IDataSerializer_Ptr> _dataSerialMap;
	std::map<std::string, IProcessorBase_Ptr> _prcMap;

private:

};

#endif