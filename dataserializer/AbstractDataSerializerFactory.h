/***********************************************************************
 * Module:  AbstractDataSerializerFactory.h
 * Author:  milk
 * Modified: 2015年9月18日 10:00:10
 * Purpose: Declaration of the class AbstractDataSerializerFactory
 ***********************************************************************/

#if !defined(__dataserializer_AbstractDataSerializerFactory_h)
#define __dataserializer_AbstractDataSerializerFactory_h

#include <memory>
#include <vector>
#include "IDataSerializerFactory.h"
#include "dataserializer_config.h"
#include "dataserializer_exp.h"

static const char* CLASS_UUID_MSG_SERIALIZER_FACTORY = "factory.message.serializer";

class DATASERIALIZER_CLASS_EXPORT AbstractDataSerializerFactory : public IDataSerializerFactory
{
public:
	void CreateDataSerializers(std::map<uint32_t, IDataSerializer_Ptr>& serializerMap);
	IDataSerializer_Ptr Find(uint32_t msgId);
	static std::shared_ptr<AbstractDataSerializerFactory> Instance(void);
	static void AddConfigForInstance(const MessageSerializerConfig& config);

protected:
	std::map<uint32_t, IDataSerializer_Ptr> _serializer_map;

private:
};

#endif