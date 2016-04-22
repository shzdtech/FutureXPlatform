/* 
 * File:   ctpexport.cpp
 * Author: milk
 * 
 * Created on 2014年10月7日, 下午4:22
 */

#include <string.h>

#include "ctpexport.h"
#include "CTPMDServiceFactory.h"
#include "CTPTradeServiceFactory.h"

extern "C" CTP_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
    void* instance = NULL;
    if (strcmp(UUID_CTP_MD_FACTORY, classUUID)==0)
        instance = new CTPMDServiceFactory();
	else if (strcmp(UUID_CTP_TRADE_FACTORY, classUUID) == 0)
		instance = new CTPTradeServiceFactory();
    return instance;
}
