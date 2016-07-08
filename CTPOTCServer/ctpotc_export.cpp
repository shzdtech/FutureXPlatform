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
#include "CTPAccountTradeServiceFactory.h"

extern "C" CTP_OTC_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
    void* instance = NULL;
	if (std::strcmp(UUID_OTC_CLIENT_FACTORY, classUUID) == 0)
		instance = new CTPOTCClientServiceFactory();
	else if (std::strcmp(UUID_OTC_TRADINGDESK_FACTORY, classUUID) == 0)
		instance = new CTPOTCTradingDeskServiceFactory();
	else if (std::strcmp(UUID_SINGLE_ACCOUNT_CTP_TRADE_FACTORY, classUUID) == 0)
		instance = new CTPAccountTradeServiceFactory();

    return instance;
}
