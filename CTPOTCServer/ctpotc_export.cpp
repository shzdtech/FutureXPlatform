/*
 * File:   ctpexport.cpp
 * Author: milk
 *
 * Created on 2014年10月7日, 下午4:22
 */

#include <string.h>

#include "ctpotc_export.h"
#include "CTPOTCServiceFactory.h"
#include "CTPOTCOptionServiceFactory.h"

extern "C" CTP_OTC_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
	void* instance = NULL;
	if (std::strcmp(UUID_OTC_CONTRACT_FACTORY, classUUID) == 0)
		instance = new CTPOTCServiceFactory();
	else if (std::strcmp(UUID_OTC_OPTION_FACTORY, classUUID) == 0)
		instance = new CTPOTCOptionServiceFactory();

	return instance;
}
