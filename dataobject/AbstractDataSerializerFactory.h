/***********************************************************************
 * Module:  AbstractDataSerializerFactory.h
 * Author:  milk
 * Modified: 2015年9月18日 10:00:10
 * Purpose: Declaration of the class AbstractDataSerializerFactory
 ***********************************************************************/

#if !defined(__dataobject_AbstractDataSerializerFactory_h)
#define __dataobject_AbstractDataSerializerFactory_h

#include <memory>
#include "IDataSerializerFactory.h"
#include "databoject_config.h"
#include "dataobject_exp.h"

static const char* CLASS_UUID_MSG_SERIALIZER_FACTORY = "factory.message.serializer";

class DATAOBJECT_CLASS_EXPORT AbstractDataSerializerFactory : public IDataSerializerFactory
{
public:
	std::map<unsigned int, IDataSerializer_Ptr> CreateDataSerializers(void);
	IDataSerializer_Ptr Find(uint32_t msgId);
	static std::shared_ptr<AbstractDataSerializerFactory> Instance(void);
	static std::vector<MessageSerializerConfig> MessageSerializerConfigs;

protected:
	std::map<uint32_t, IDataSerializer_Ptr> _serializer_map;

private:
};

#endif