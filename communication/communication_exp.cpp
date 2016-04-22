#include "communication_exp.h"
#include "ASIOTCPServer.h"

extern "C" COMMUNICATION_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
	void* instance = NULL;
	if (strcmp(UUID_ASIOTCP_LITE_SERVER, classUUID) == 0)
		instance = new ASIOTCPServer;
	return instance;
}
