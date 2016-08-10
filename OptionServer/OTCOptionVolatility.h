#if !defined(__OTCOPTION_OTCOptionVolatility_h)
#define __OTCOPTION_OTCOptionVolatility_h

#include "../message/NopHandler.h"
#include "otcoption_export.h"

class OPTION_SERVER_CLASS_EXPORT OTCOptionVolatility : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
};

#endif