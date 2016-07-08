/* 
 * File:   ctpexport.h
 * Author: milk
 *
 * Created on 2014年10月7日, 下午4:22
 */

#ifndef __CTP_OTC_EXPORT_H
#define	__CTP_OTC_EXPORT_H

#if defined (_MSC_VER)
#if defined (CTP_OTC_LIBRARY_EXPORTS)
#define CTP_OTC_CLASS_EXPORT __declspec(dllexport)
#else
#define CTP_OTC_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define CTP_OTC_CLASS_EXPORT
#endif

static const char* UUID_OTC_CLIENT_FACTORY = "factory.otc.client.service";
static const char* UUID_OTC_TRADINGDESK_FACTORY = "factory.otc.tradingdesk.service";
static const char* UUID_SINGLE_ACCOUNT_CTP_TRADE_FACTORY = "factory.singleaccount.ctp.trade.service";

#endif	/* CTPEXPORT_H */

