/***********************************************************************
 * Module:  CTPQueryTrade.h
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Declaration of the class CTPQueryTrade
 ***********************************************************************/

#if !defined(__CTP_CTPQueryTrade_h)
#define __CTP_CTPQueryTrade_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryTrade : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif