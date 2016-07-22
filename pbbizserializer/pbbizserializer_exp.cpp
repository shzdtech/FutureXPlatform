#include <string.h>

#include "pbbizserializer_exp.h"
#include "PBBizMessageSerializerFactory.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"


extern "C" PBBIZSERIALIZER_CLASS_EXPORTS void* CreateInstance(const char* classUUID) {
	void* instance = NULL;
	if (std::strcmp(CLASS_UUID_MSG_BIZ_SERIALIZER_FACTORY, classUUID) == 0)
		instance = new PBBizMessageSerializerFactory();

	return instance;
}
