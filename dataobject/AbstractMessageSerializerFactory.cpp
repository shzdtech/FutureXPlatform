/***********************************************************************
 * Module:  AbstractMessageSerializerFactory.cpp
 * Author:  milk
 * Modified: 2015年9月18日 10:00:10
 * Purpose: Implementation of the class AbstractMessageSerializerFactory
 ***********************************************************************/

#include "AbstractMessageSerializerFactory.h"
#include "../dynamicloader/ConfigBasedCreator.h"

MessageSerializerConfig AbstractMessageSerializerFactory::DefaultMessageSerializerConfig;

////////////////////////////////////////////////////////////////////////
// Name:       AbstractMessageSerializerFactory::Instance()
// Purpose:    Implementation of AbstractMessageSerializerFactory::Instance()
// Return:     IMessageSerializerFactory_Ptr
////////////////////////////////////////////////////////////////////////

std::shared_ptr<AbstractMessageSerializerFactory> AbstractMessageSerializerFactory::Instance(void)
{
	static std::shared_ptr<AbstractMessageSerializerFactory> instance(
		std::static_pointer_cast<AbstractMessageSerializerFactory>
		(ConfigBasedCreator::CreateInstance(
			DefaultMessageSerializerConfig.MODULE_UUID,
			DefaultMessageSerializerConfig.MODULE_PATH,
		CLASS_UUID_MSG_SERIALIZER_FACTORY)));

	return instance;
}


////////////////////////////////////////////////////////////////////////
// Name:       AbstractMessageSerializerFactory::Find(unsigned int msgId)
// Purpose:    Implementation of AbstractMessageSerializerFactory::Find()
// Parameters:
// - msgId
// Return:     IDataSerializer_Ptr
////////////////////////////////////////////////////////////////////////

IDataSerializer_Ptr AbstractMessageSerializerFactory::Find(unsigned int msgId)
{
	return _serializer_map[msgId];
}