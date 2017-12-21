#if !defined(__CTP_CTPSyncTrade_h)
#define __CTP_CTPSyncTrade_h

#include "../message/NopHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPSyncTrade : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
};

#endif
