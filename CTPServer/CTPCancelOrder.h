/***********************************************************************
 * Module:  CTPCancleOrder.h
 * Author:  milk
 * Modified: 2015年10月26日 11:31:17
 * Purpose: Declaration of the class CTPCancleOrder
 ***********************************************************************/

#if !defined(__CTP_CTPCancleOrder_h)
#define __CTP_CTPCancleOrder_h

#include "../message/MessageHandler.h"

class CTPCancelOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif