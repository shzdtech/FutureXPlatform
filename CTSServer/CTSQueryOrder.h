/***********************************************************************
 * Module:  CTSQueryOrder.h
 * Author:  milk
 * Modified: 2015年12月10日 12:57:18
 * Purpose: Declaration of the class CTSQueryOrder
 ***********************************************************************/

#if !defined(__CTSServer_CTSQueryOrder_h)
#define __CTSServer_CTSQueryOrder_h

#include "../message/IMessageHandler.h"

class CTSQueryOrder : public IMessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif