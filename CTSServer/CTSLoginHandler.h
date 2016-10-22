/***********************************************************************
 * Module:  CTSLoginHandler.h
 * Author:  milk
 * Modified: 2015年12月9日 15:51:46
 * Purpose: Declaration of the class CTSLoginHandler
 ***********************************************************************/

#if !defined(__CTSServer_CTSLoginHandler_h)
#define __CTSServer_CTSLoginHandler_h

#include "../message/IMessageHandler.h"

class CTSLoginHandler : public IMessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session);

protected:

private:

};

#endif