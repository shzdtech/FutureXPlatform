/***********************************************************************
 * Module:  AbstractDataSerializerFactory.cpp
 * Author:  milk
 * Modified: 2015年9月18日 10:00:10
 * Purpose: Implementation of the class AbstractDataSerializerFactory
 ***********************************************************************/

#include "AbstractDataSerializerFactory.h"
#include "../dynamicloader/ConfigBasedCreator.h"

std::vector<MessageSerializerConfig>
AbstractDataSerializerFactory::MessageSerializerConfigs;

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
		for (auto& config : MessageSerializerConfigs)
		{
			std::shared_ptr<AbstractDataSerializerFactory> serializerFactory(
				std::static_pointer_cast<AbstractDataSerializerFactory>
				(ConfigBasedCreator::CreateInstance
				(config.MODULE_UUID, config.MODULE_PATH, config.CLASS_UUID)));
			auto serializerMap = serializerFactory->CreateDataSerializers();
			instance->_serializer_map.insert(serializerMap.begin(), serializerMap.end());
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

std::map<unsigned int, IDataSerializer_Ptr>
AbstractDataSerializerFactory::CreateDataSerializers(void)
{
	return _serializer_map;
}