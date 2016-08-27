/*
* File:   CTPConstant.h
* Author: milk
*
* Created on 2015��10��22��, ����23:22
*/

#ifndef __CTP_CONSTANT_H
#define	__CTP_CONSTANT_H
#include <string>

static const std::string EXCHANGE_CTP = "ctp";
static const std::string EXCHANGE_EMPTY = "";
static const std::string CTP_MD_BROKERID = "CTP.MD.LOGIN.BROKERID";
static const std::string CTP_MD_USERID = "CTP.MD.LOGIN.USERID";
static const std::string CTP_MD_PASSWORD = "CTP.MD.LOGIN.PASSWORD";
static const std::string CTP_MD_SERVER = "CTP.MD.SERVER";
static const std::string CTP_TRADER_BROKERID = "CTP.TRADER.LOGIN.BROKERID";
static const std::string CTP_TRADER_USERID = "CTP.TRADER.LOGIN.USERID";
static const std::string CTP_TRADER_PASSWORD = "CTP.TRADER.LOGIN.PASSWORD";
static const std::string CTP_TRADER_SERVER = "CTP.TRADER.SERVER";

static const char* UUID_MICROFUTURE_CTP = "M.F.0.1";
static const char* FMT_PADDING_ORDERREF = "%llu";
static const char* FMT_PADDING_ORDERSYSID = "%12llu";

#endif	/* __CTP_CONSTANT_H */

