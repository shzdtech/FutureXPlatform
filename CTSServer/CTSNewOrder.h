/***********************************************************************
 * Module:  CTSNewOrder.h
 * Author:  milk
 * Modified: 2015年12月9日 18:33:58
 * Purpose: Declaration of the class CTSNewOrder
 ***********************************************************************/

#if !defined(__CTSServer_CTSNewOrder_h)
#define __CTSServer_CTSNewOrder_h

#include "../message/IMessageHandler.h"

class CTSNewOrder : public IMessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif