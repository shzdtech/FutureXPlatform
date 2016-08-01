/***********************************************************************
 * Module:  CTPOTCTradingDeskLogin.h
 * Author:  milk
 * Modified: 2015年9月3日 13:30:09
 * Purpose: Declaration of the class CTPOTCTradingDeskLogin
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradingDeskLogin_h)
#define __CTPOTC_CTPOTCTradingDeskLogin_h

#include "CTPOTCLogin.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradingDeskLogin : public CTPOTCLogin
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif