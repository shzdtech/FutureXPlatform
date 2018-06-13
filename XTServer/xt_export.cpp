/* 
 * File:   ctpexport.cpp
 * Author: milk
 * 
 * Created on 2014年10月7日, 下午4:22
 */

#include <string.h>

#include "xt_export.h"
#include "CTPMDServiceFactory.h"
#include "CTPTradeServiceFactory.h"
#include "CTPTradeServiceSAFactory.h"
#include "CTPMDServiceSAFactory.h"

extern "C" XT_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
    void* instance = NULL;
    if (strcmp(UUID_CTP_MD_FACTORY, classUUID)==0)
        instance = new CTPMDServiceFactory();
	else if (strcmp(UUID_CTP_TRADE_FACTORY, classUUID) == 0)
		instance = new CTPTradeServiceFactory();
	else if (std::strcmp(UUID_SINGLE_ACCOUNT_CTP_TRADE_FACTORY, classUUID) == 0)
		instance = new CTPTradeServiceSAFactory();
	else if (std::strcmp(UUID_SINGLE_ACCOUNT_CTP_MARKETDATA_FACTORY, classUUID) == 0)
		instance = new CTPMDServiceSAFactory();

    return instance;
}
