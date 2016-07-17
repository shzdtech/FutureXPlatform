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
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif