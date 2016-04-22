#include <string.h>

#include "pbserializer_exp.h"
#include "PBMessageSerializerFactory.h"
#include "../dataobject/AbstractMessageSerializerFactory.h"


extern "C" PBSERIALIZER_CLASS_EXPORTS void* CreateInstance(const char* classUUID) {
	void* instance = NULL;
	if (std::strcmp(CLASS_UUID_MSG_SERIALIZER_FACTORY, classUUID) == 0)
		instance = new PBMessageSerializerFactory();

	return instance;
}
