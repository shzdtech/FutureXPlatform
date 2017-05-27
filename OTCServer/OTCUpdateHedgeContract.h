#if !defined(__OTCServer_OTCUpdateHedgeContract_h)
#define __OTCServer_OTCUpdateHedgeContract_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCUpdateHedgeContract : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	
};

#endif