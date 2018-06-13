/* 
 * File:   ctpexport.h
 * Author: milk
 *
 * Created on 2014年10月7日, 下午4:22
 */

#ifndef __XTEXPORT_H
#define	__XTEXPORT_H

#if defined (_MSC_VER)
#if defined (CTP_LIBRARY_EXPORTS)
#define XT_CLASS_EXPORT __declspec(dllexport)
#else
#define XT_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define XT_CLASS_EXPORT
#endif

static const char* UUID_CTP_MD_FACTORY = "factory.ctp.marketdata.service";
static const char* UUID_CTP_TRADE_FACTORY = "factory.ctp.trade.service";
static const char* UUID_SINGLE_ACCOUNT_CTP_TRADE_FACTORY = "factory.singleaccount.ctp.trade.service";
static const char* UUID_SINGLE_ACCOUNT_CTP_MARKETDATA_FACTORY = "factory.singleaccount.ctp.marketdata.service";

#endif	/* CTPEXPORT_H */

