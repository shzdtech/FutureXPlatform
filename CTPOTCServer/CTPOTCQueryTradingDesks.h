/***********************************************************************
 * Module:  CTPOTCTradingDesks.h
 * Author:  milk
 * Modified: 2015年8月27日 20:23:20
 * Purpose: Declaration of the class CTPOTCTradingDesks
 ***********************************************************************/

#if !defined(__CTPOTC_CTPQueryTradingDesks_h)
#define __CTPOTC_CTPQueryTradingDesks_h

#include "../message/NopHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCQueryTradingDesks : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif