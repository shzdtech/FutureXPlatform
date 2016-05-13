/***********************************************************************
 * Module:  AbstractDataSerializerFactory.cpp
 * Author:  milk
 * Modified: 2015年9月18日 10:00:10
 * Purpose: Implementation of the class AbstractDataSerializerFactory
 ***********************************************************************/

#include "AbstractDataSerializerFactory.h"
#include "../dynamicloader/ConfigBasedCreator.h"

MessageSerializerConfig AbstractDataSerializerFactory::DefaultMessageSerializerConfig;

////////////////////////////////////////////////////////////////////////
// Name:       AbstractDataSerializerFactory::Instance()
// Purpose:    Implementation of AbstractDataSerializerFactory::Instance()
// Return:     IMessageSerializerFactory_Ptr
////////////////////////////////////////////////////////////////////////

std::shared_ptr<AbstractDataSerializerFactory> AbstractDataSerializerFactory::Instance(void)
{
	static std::shared_ptr<AbstractDataSerializerFactory> instance(
		std::static_pointer_cast<AbstractDataSerializerFactory>
		(ConfigBasedCreator::CreateInstance(
			DefaultMessageSerializerConfig.MODULE_UUID,
			DefaultMessageSerializerConfig.MODULE_PATH,
		CLASS_UUID_MSG_SERIALIZER_FACTORY)));

	return instance;
}


////////////////////////////////////////////////////////////////////////
// Name:       AbstractDataSerializerFactory::Find(unsigned int msgId)
// Purpose:    Implementation of AbstractDataSerializerFactory::Find()
// Parameters:
// - msgId
// Return:     IDataSerializer_Ptr
////////////////////////////////////////////////////////////////////////

IDataSerializer_Ptr AbstractDataSerializerFactory::Find(unsigned int msgId)
{
	return _serializer_map[msgId];
}