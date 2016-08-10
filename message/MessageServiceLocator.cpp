/***********************************************************************
 * Module:  MessageRouter.cpp
 * Author:  milk
 * Modified: 2014年10月6日 13:03:58
 * Purpose: Implementation of the class MessageRouter
 ***********************************************************************/

#include "MessageServiceLocator.h"
#include "IMessageServiceFactory.h"

////////////////////////////////////////////////////////////////////////
// Name:       MessageRouter::getMsgHandler(uint msgId)
// Purpose:    Implementation of MessageRouter::getMsgHandler()
// Parameters:
// - msgId
// Return:     messgehandler_ptr
////////////////////////////////////////////////////////////////////////

IMessageHandler_Ptr MessageServiceLocator::FindMessageHandler(uint msgId) {
	IMessageHandler_Ptr ret;
    auto itr = _msghdlMap.find(msgId);
    if (itr != _msghdlMap.end())
        ret = itr->second;

    return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageRouter::FindDataSerializer(uint msgId)
// Purpose:    Implementation of MessageRouter::FindDataSerializer()
// Parameters:
// - msgId
// Return:     IDataSerializer_Ptr
////////////////////////////////////////////////////////////////////////

IDataSerializer_Ptr MessageServiceLocator::FindDataSerializer(uint msgId) {
    IDataSerializer_Ptr ret;
    auto itr = _dataSerialMap.find(msgId);
	if (itr != _dataSerialMap.end())
        ret = itr->second;

    return ret;
}

IMessageProcessor_Ptr MessageServiceLocator::GetWorkerProcessor(void)
{
	return _workProcessor;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageRouter::MessageRouter(std::map<int, messgehandler_ptr> msghdlMap, std::map<int, IDataSerializer_Ptr> msgsrlMap)
// Purpose:    Implementation of MessageRouter::MessageRouter()
// Parameters:
// - msghdlMap
// - msgsrlMap
// Return:     
////////////////////////////////////////////////////////////////////////

MessageServiceLocator::MessageServiceLocator(IMessageServiceFactory_Ptr msgsvc_fac, IServerContext* serverCtx)
{
	_workProcessor = msgsvc_fac->CreateWorkerProcessor(serverCtx);
    _msghdlMap = msgsvc_fac->CreateMessageHandlers(serverCtx);
	_dataSerialMap = msgsvc_fac->CreateDataSerializers(serverCtx);
}



std::map<uint, IMessageHandler_Ptr>& MessageServiceLocator::AllMessageHandler(void)
{
	return _msghdlMap;
}

std::map<uint, IDataSerializer_Ptr>& MessageServiceLocator::AllDataSerializer(void)
{
	return _dataSerialMap;
}