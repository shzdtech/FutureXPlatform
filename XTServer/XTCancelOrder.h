/***********************************************************************
 * Module:  CTPCancleOrder.h
 * Author:  milk
 * Modified: 2015年10月26日 11:31:17
 * Purpose: Declaration of the class CTPCancleOrder
 ***********************************************************************/

#if !defined(__XT_CTPCancleOrder_h)
#define __XT_CTPCancleOrder_h

#include "../message/MessageHandler.h"

class CTPCancelOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif