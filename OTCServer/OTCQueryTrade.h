/***********************************************************************
 * Module:  OTCQueryOrder.h
 * Author:  milk
 * Modified: 2015年10月15日 16:08:36
 * Purpose: Declaration of the class OTCQueryOrder
 ***********************************************************************/

#if !defined(__OTCServer_OTCQueryTrade_h)
#define __OTCServer_OTCQueryTrade_h

#include "../message/MessageHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCQueryTrade : public MessageHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif