#if !defined(__OTCServer_OTCNewPortfolio_h)
#define __OTCServer_OTCNewPortfolio_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCNewPortfolio : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	
};

#endif