/* 
 * File:   ctpexport.h
 * Author: milk
 *
 * Created on 2014年10月7日, 下午4:22
 */

#ifndef __XTEXPORT_H
#define	__XTEXPORT_H

#if defined (_MSC_VER)
#if defined (XT_LIBRARY_EXPORTS)
#define XT_CLASS_EXPORT __declspec(dllexport)
#else
#define XT_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define XT_CLASS_EXPORT
#endif

static const char* UUID_XT_TRADE_FACTORY = "factory.xt.trade.service";
static const char* UUID_SINGLE_ACCOUNT_XT_TRADE_FACTORY = "factory.singleaccount.xt.trade.service";

static const char* UUID_XT_OTC_OPTION_FACTORY = "factory.xt.otc.option";
static const char* UUID_XT_OTC_OPTION_TRADE_FACTORY = "factory.xt.otc.option.trade";

#endif	/* CTPEXPORT_H */

