#if !defined(__CTPOTC_OTCOptionVolatility_h)
#define __CTPOTC_OTCOptionVolatility_h

#include "../message/NopHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT OTCOptionVolatility : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
};

#endif