/* 
 * File:   ctpexport.cpp
 * Author: milk
 * 
 * Created on 2014年10月7日, 下午4:22
 */

#include <string.h>

#include "xt_export.h"

#include "XTTradeServiceFactory.h"
#include "XTOTCTradeServiceFactory.h"
#include "XTOTCOptionServiceFactory.h"

extern "C" XT_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
    void* instance = NULL;
    if (strcmp(UUID_CTP_MD_FACTORY, classUUID)==0)
        instance = new CTPMDServiceFactory();
	else if (strcmp(UUID_XT_TRADE_FACTORY, classUUID) == 0)
		instance = new XTTradeServiceFactory();
	else if (std::strcmp(UUID_XT_OTC_OPTION_FACTORY, classUUID) == 0)
		instance = new XTOTCOptionServiceFactory();
	else if (std::strcmp(UUID_XT_OTC_OPTION_TRADE_FACTORY, classUUID) == 0)
		instance = new XTOTCTradeServiceFactory();

    return instance;
}
