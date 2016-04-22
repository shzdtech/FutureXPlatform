/* 
 * File:   ctpexport.cpp
 * Author: milk
 * 
 * Created on 2014年10月7日, 下午4:22
 */

#include <string.h>

#include "ctpotc_export.h"
#include "CTPOTCClientServiceFactory.h"
#include "CTPOTCTradingDeskServiceFactory.h"

extern "C" CTP_OTC_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
    void* instance = NULL;
	if (strcmp(UUID_OTC_CLIENT_FACTORY, classUUID) == 0)
		instance = new CTPOTCClientServiceFactory();
	else if (strcmp(UUID_OTC_TRADINGDESK_FACTORY, classUUID) == 0)
		instance = new CTPOTCTradingDeskServiceFactory();
    return instance;
}
