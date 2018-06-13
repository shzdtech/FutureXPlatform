/***********************************************************************
 * Module:  CTPQueryOrder.h
 * Author:  milk
 * Modified: 2015年7月11日 13:18:21
 * Purpose: Declaration of the class CTPQueryOrder
 ***********************************************************************/

#if !defined(__XT_CTPQueryOrder_h)
#define __XT_CTPQueryOrder_h

#include "../message/MessageHandler.h"
#include "xt_export.h"

class XT_CLASS_EXPORT CTPQueryOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif