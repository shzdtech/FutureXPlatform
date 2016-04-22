/***********************************************************************
 * Module:  CTPOTCQueryOrder.h
 * Author:  milk
 * Modified: 2015年10月15日 16:08:36
 * Purpose: Declaration of the class CTPOTCQueryOrder
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCQueryOrder_h)
#define __CTPOTC_CTPOTCQueryOrder_h

#include "../message/MessageHandler.h"

class CTPOTCQueryOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif