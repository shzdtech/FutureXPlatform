/***********************************************************************
 * Module:  OTCNewOrder.h
 * Author:  milk
 * Modified: 2015年10月14日 0:08:44
 * Purpose: Declaration of the class OTCNewOrder
 ***********************************************************************/

#if !defined(__OTCServer_OTCNewOrder_h)
#define __OTCServer_OTCNewOrder_h

#include "../message/MessageHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCNewOrder : public MessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif