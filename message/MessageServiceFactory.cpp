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

std::map<uint, IMessageHandler_Ptr> MessageServiceFactory::CreateMessageHandlers(void)
{
	return std::map<uint, IMessageHandler_Ptr>();
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of MessageServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> MessageServiceFactory::CreateDataSerializers(void)
{
	return std::map<uint, IDataSerializer_Ptr>();
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::CreateWorkProcessor()
// Purpose:    Implementation of MessageServiceFactory::CreateWorkProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IProcessorBase_Ptr> MessageServiceFactory::CreateWorkProcessor(void)
{
	return std::map<uint, IProcessorBase_Ptr>();
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of MessageServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr MessageServiceFactory::CreateMessageProcessor(void)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       MessageServiceFactory::Load(const std::string& configFile, const std::string& param)
// Purpose:    Implementation of MessageServiceFactory::Load()
// Parameters:
// - configFile
// - param
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool MessageServiceFactory::Load(const std::string& configFile, const std::string& param)
{
	bool ret = false;
	auto reader = AbstractConfigReaderFactory::CreateConfigReader();
	if (reader->LoadFromFile(configFile)) {
		std::map<std::string, std::string> cfgMap;
		if (reader->getMap(param, cfgMap)) {
			_configMap = cfgMap;
			ret = true;
		}
	}
	return ret;
}