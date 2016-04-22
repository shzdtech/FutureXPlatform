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

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceLocator::FindWorkProccessor(uint processorID)
// Purpose:    Implementation of MessageServiceLocator::FindWorkProccessor()
// Parameters:
// - processorID
// Return:     IProcessorBase_Ptr
////////////////////////////////////////////////////////////////////////

IProcessorBase_Ptr MessageServiceLocator::FindWorkProccessor(uint processorID)
{
	IProcessorBase_Ptr ret;
	auto itr = _prcMap.find(processorID);
	if (itr != _prcMap.end())
		ret = itr->second;

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageRouter::MessageRouter(std::map<int, messgehandler_ptr> msghdlMap, std::map<int, IDataSerializer_Ptr> msgsrlMap)
// Purpose:    Implementation of MessageRouter::MessageRouter()
// Parameters:
// - msghdlMap
// - msgsrlMap
// Return:     
////////////////////////////////////////////////////////////////////////

MessageServiceLocator::MessageServiceLocator(IMessageServiceFactory_Ptr msgsvc_fac) {
    _msghdlMap = msgsvc_fac->CreateMessageHandlers();
    _dataSerialMap = msgsvc_fac->CreateDataSerializers();
	_prcMap = msgsvc_fac->CreateWorkProcessor();
}



std::map<uint, IMessageHandler_Ptr>& MessageServiceLocator::AllMessageHandler(void)
{
	return _msghdlMap;
}

std::map<uint, IDataSerializer_Ptr>& MessageServiceLocator::AllDataSerializer(void)
{
	return _dataSerialMap;
}

std::map<uint, IProcessorBase_Ptr>& MessageServiceLocator::AllWorkProcessor(void)
{
	return _prcMap;
}