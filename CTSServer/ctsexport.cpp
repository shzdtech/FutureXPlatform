/*
* File:   ctsexport.cpp
* Author: milk
*
* Created on 2015年10月7日, 下午4:22
*/
#include <string.h>

#include "ctsexport.h"

#include "CTSServiceFactory.h"

const char* UUID_CTS_FACTORY = "factory.cts.service";

extern "C" CTS_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
	void* instance = NULL;
	if (strcmp(UUID_CTS_FACTORY, classUUID) == 0)
		instance = new CTSServiceFactory();
	return instance;
}
