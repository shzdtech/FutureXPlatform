/***********************************************************************
 * Module:  CTPOTCNewOrder.h
 * Author:  milk
 * Modified: 2015年10月14日 0:08:44
 * Purpose: Declaration of the class CTPOTCNewOrder
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCNewOrder_h)
#define __CTPOTC_CTPOTCNewOrder_h

#include "../message/MessageHandler.h"

class CTPOTCNewOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif