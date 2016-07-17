/***********************************************************************
 * Module:  CTPOTCTradingDesks.h
 * Author:  milk
 * Modified: 2015年8月27日 20:23:20
 * Purpose: Declaration of the class CTPOTCTradingDesks
 ***********************************************************************/

#if !defined(__CTPOTCQueryTradingDesks_h)
#define __CTPOTCQueryTradingDesks_h

#include "../message/NopHandler.h"

class CTPOTCQueryTradingDesks : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif