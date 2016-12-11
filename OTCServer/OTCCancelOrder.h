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
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif