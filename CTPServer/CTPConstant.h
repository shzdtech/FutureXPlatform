/*
* File:   CTPConstant.h
* Author: milk
*
* Created on 2015年10月22日, 下午23:22
*/

#ifndef __CTP_CONSTANT_H
#define	__CTP_CONSTANT_H
#include <string>

static const std::string EXCHANGE_SHFE = "SHFE";
static const std::string EXCHANGE_CTP = "ctp";
static const std::string EXCHANGE_EMPTY;
static const std::string CTP_MD_BROKERID = "CTP.MD.LOGIN.BROKERID";
static const std::string CTP_MD_USERID = "CTP.MD.LOGIN.USERID";
static const std::string CTP_MD_PASSWORD = "CTP.MD.LOGIN.PASSWORD";
static const std::string CTP_MD_SERVER = "CTP.MD.SERVER";
static const std::string CTP_TRADER_BROKERID = "CTP.TRADER.LOGIN.BROKERID";
static const std::string CTP_TRADER_USERID = "CTP.TRADER.LOGIN.USERID";
static const std::string CTP_TRADER_PASSWORD = "CTP.TRADER.LOGIN.PASSWORD";
static const std::string CTP_TRADER_SERVER = "CTP.TRADER.SERVER";

static const std::string CTP_AUTH_CODE = "CTP.AUTH.CODE";
static const std::string CTP_PRODUCT_INFO = "CTP.PRODUCT.INFO";

static const std::string SYNC_POSITION_FROMDB = "SYNC.POSITION.FROMDB";

static const char* UUID_MICROFUTURE_CTP = "M.F.0.1";
static const char* FMT_ORDERREF = "%u";
static const char* FMT_ORDERSYSID = "%12llu";

#endif	/* __CTP_CONSTANT_H */

