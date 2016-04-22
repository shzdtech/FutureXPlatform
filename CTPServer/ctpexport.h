/* 
 * File:   ctpexport.h
 * Author: milk
 *
 * Created on 2014年10月7日, 下午4:22
 */

#ifndef __CTPEXPORT_H
#define	__CTPEXPORT_H

#if defined (_MSC_VER)
#if defined (CTP_LIBRARY_EXPORTS)
#define CTP_CLASS_EXPORT __declspec(dllexport)
#else
#define CTP_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define CTP_CLASS_EXPORT
#endif

static const char* UUID_CTP_MD_FACTORY = "factory.ctp.marketdata.service";
static const char* UUID_CTP_TRADE_FACTORY = "factory.ctp.trade.service";

#endif	/* CTPEXPORT_H */

