/***********************************************************************
 * Module:  AbstractDataSerializerFactory.cpp
 * Author:  milk
 * Modified: 2015年9月18日 10:00:10
 * Purpose: Implementation of the class AbstractDataSerializerFactory
 ***********************************************************************/

#include "AbstractDataSerializerFactory.h"
#include "../dynamicloader/ConfigBasedCreator.h"

static std::vector<MessageSerializerConfig> messageSerializerConfigs;

////////////////////////////////////////////////////////////////////////
// Name:       AbstractDataSerializerFactory::Instance()
// Purpose:    Implementation of AbstractDataSerializerFactory::Instance()
// Return:     IMessageSerializerFactory_Ptr
////////////////////////////////////////////////////////////////////////

std::shared_ptr<AbstractDataSerializerFactory>
AbstractDataSerializerFactory::Instance(void)
{
	static bool once_flag = true;
	static std::shared_ptr<AbstractDataSerializerFactory> 
		instance(new AbstractDataSerializerFactory);

	if (once_flag)
	{
		for (auto& config : messageSerializerConfigs)
		{
			auto serializerFactory_ptr = std::static_pointer_cast<AbstractDataSerializerFactory>
				(ConfigBasedCreator::CreateInstance(config.MODULE_UUID, config.MODULE_PATH, config.CLASS_UUID));
			serializerFactory_ptr->CreateDataSerializers(instance->_serializer_map);
		}
		once_flag = false;
	}

	return instance;
}


////////////////////////////////////////////////////////////////////////
// Name:       AbstractDataSerializerFactory::Find(uint32_t msgId)
// Purpose:    Implementation of AbstractDataSerializerFactory::Find()
// Parameters:
// - msgId
// Return:     IDataSerializer_Ptr
////////////////////////////////////////////////////////////////////////

IDataSerializer_Ptr AbstractDataSerializerFactory::Find(uint32_t msgId)
{
	return _serializer_map[msgId];
}

void AbstractDataSerializerFactory::CreateDataSerializers(std::map<uint32_t, IDataSerializer_Ptr>& serializerMap)
{
	serializerMap.insert(_serializer_map.begin(), _serializer_map.end());
}

void AbstractDataSerializerFactory::AddConfigForInstance(const MessageSerializerConfig& config)
{
	messageSerializerConfigs.push_back(config);
}