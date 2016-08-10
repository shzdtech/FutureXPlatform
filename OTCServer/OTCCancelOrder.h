/***********************************************************************
 * Module:  OTCCancelOrder.h
 * Author:  milk
 * Modified: 2015年10月14日 0:09:09
 * Purpose: Declaration of the class OTCCancelOrder
 ***********************************************************************/

#if !defined(__OTCServer_OTCCancelOrder_h)
#define __OTCServer_OTCCancelOrder_h

#include "../message/MessageHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCCancelOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif