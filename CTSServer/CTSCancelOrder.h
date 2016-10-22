/***********************************************************************
 * Module:  CTSCancelOrder.h
 * Author:  milk
 * Modified: 2015年12月10日 12:56:23
 * Purpose: Declaration of the class CTSCancelOrder
 ***********************************************************************/

#if !defined(__CTSServer_CTSCancelOrder_h)
#define __CTSServer_CTSCancelOrder_h

#include "../message/IMessageHandler.h"

class CTSCancelOrder : public IMessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif