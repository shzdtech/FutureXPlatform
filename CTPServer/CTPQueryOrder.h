/***********************************************************************
 * Module:  CTPQueryOrder.h
 * Author:  milk
 * Modified: 2015年7月11日 13:18:21
 * Purpose: Declaration of the class CTPQueryOrder
 ***********************************************************************/

#if !defined(__CTP_CTPQueryOrder_h)
#define __CTP_CTPQueryOrder_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif