/***********************************************************************
 * Module:  XTNewOrder.h
 * Author:  milk
 * Modified: 2015年10月20日 22:38:54
 * Purpose: Declaration of the class XTNewOrder
 ***********************************************************************/

#if !defined(__XT_XTNewOrder_h)
#define __XT_XTNewOrder_h

#include "../message/MessageHandler.h"

class XTNewOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif