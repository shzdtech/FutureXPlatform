/***********************************************************************
 * Module:  MessageServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年9月2日 13:26:31
 * Purpose: Implementation of the class MessageServiceFactory
 ***********************************************************************/

#include "MessageServiceFactory.h"
#include "../configuration/AbstractConfigReaderFactory.h"

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of MessageServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> MessageServiceFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
	return std::map<uint, IMessageHandler_Ptr>();
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of MessageServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> MessageServiceFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	return std::map<uint, IDataSerializer_Ptr>();
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::CreateWorkerProcessor()
// Purpose:    Implementation of MessageServiceFactory::CreateWorkerProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr MessageServiceFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of MessageServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr MessageServiceFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::LoadParams(const std::string& configFile, const std::string& param)
// Purpose:    Implementation of MessageServiceFactory::LoadParams()
// Parameters:
// - configFile
// - param
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool MessageServiceFactory::LoadParams(const std::string& configFile, const std::string& section)
{
	bool ret = true;
	if (auto reader = AbstractConfigReaderFactory::OpenConfigReader(configFile.data())) {
		std::map<std::string, std::string> cfgMap;
		if (reader->getMap(section, cfgMap)) {
			_configMap.insert(cfgMap.begin(), cfgMap.end());
		}
	}
	return ret;
}


void MessageServiceFactory::SetServerContext(IServerContext * serverCtx)
{
	_serverCtx = serverCtx;
	for(auto& pair : _configMap)
		_serverCtx->setConfigVal(pair.first, pair.second);
}
