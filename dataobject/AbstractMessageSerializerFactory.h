/***********************************************************************
 * Module:  AbstractMessageSerializerFactory.h
 * Author:  milk
 * Modified: 2015年9月18日 10:00:10
 * Purpose: Declaration of the class AbstractMessageSerializerFactory
 ***********************************************************************/

#if !defined(__dataobject_AbstractMessageSerializerFactory_h)
#define __dataobject_AbstractMessageSerializerFactory_h

#include <memory>
#include "IMessageSerializerFactory.h"
#include "databoject_config.h"
#include "dataobject_exp.h"

static const char* CLASS_UUID_MSG_SERIALIZER_FACTORY = "factory.message.serializer";

class DATAOBJECT_CLASS_EXPORT AbstractMessageSerializerFactory : public IMessageSerializerFactory
{
public:
	//virtual std::map<unsigned int, IDataSerializer_Ptr> CreateDataSerializers(void) = 0;
	IDataSerializer_Ptr Find(unsigned int msgId);
	static std::shared_ptr<AbstractMessageSerializerFactory> Instance(void);
	static MessageSerializerConfig DefaultMessageSerializerConfig;

protected:
	std::map<unsigned int, IDataSerializer_Ptr> _serializer_map;

private:
};

#endif