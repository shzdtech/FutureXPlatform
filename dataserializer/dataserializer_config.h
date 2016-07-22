#ifndef __dataserializer_config_h_
#define __dataserializer_config_h_

#include <string>

struct MessageSerializerConfig
{
	std::string MODULE_UUID = "module.message.serializer";
	std::string MODULE_PATH = "pbserializer.dll";
	std::string CLASS_UUID = "factory.message.serializer";
};

#endif