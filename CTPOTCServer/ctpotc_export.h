/* 
 * File:   ctpexport.h
 * Author: milk
 *
 * Created on 2014年10月7日, 下午4:22
 */

#ifndef __CTPOTC_OTC_EXPORT_H
#define	__CTPOTC_OTC_EXPORT_H

#if defined (_MSC_VER)
#if defined (CTP_OTC_LIBRARY_EXPORTS)
#define CTP_OTC_CLASS_EXPORT __declspec(dllexport)
#else
#define CTP_OTC_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define CTP_OTC_CLASS_EXPORT
#endif

static const char* UUID_OTC_CONTRACT_FACTORY = "factory.otc.contract";
static const char* UUID_OTC_TRADE_FACTORY = "factory.otc.trade";

static const char* UUID_OTC_OPTION_FACTORY = "factory.otc.option";
static const char* UUID_OTC_OPTION_TRADE_FACTORY = "factory.otc.option.trade";


#endif	/* CTPEXPORT_H */

