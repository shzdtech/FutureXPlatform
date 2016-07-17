/***********************************************************************
 * Module:  CTPNewOrder.h
 * Author:  milk
 * Modified: 2015年10月20日 22:38:54
 * Purpose: Declaration of the class CTPNewOrder
 ***********************************************************************/

#if !defined(__CTP_CTPNewOrder_h)
#define __CTP_CTPNewOrder_h

#include "../message/MessageHandler.h"

class CTPNewOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif