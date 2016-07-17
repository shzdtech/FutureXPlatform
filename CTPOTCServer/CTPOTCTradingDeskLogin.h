/***********************************************************************
 * Module:  CTPOTCTradingDeskLogin.h
 * Author:  milk
 * Modified: 2015年9月3日 13:30:09
 * Purpose: Declaration of the class CTPOTCTradingDeskLogin
 ***********************************************************************/

#if !defined(__CTP_CTPOTCTradingDeskLogin_h)
#define __CTP_CTPOTCTradingDeskLogin_h

#include "CTPOTCLogin.h"

class CTPOTCTradingDeskLogin : public CTPOTCLogin
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif