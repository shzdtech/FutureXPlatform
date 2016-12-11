/***********************************************************************
 * Module:  CTPQueryInstrument.h
 * Author:  milk
 * Modified: 2015年7月11日 13:06:13
 * Purpose: Declaration of the class CTPQueryInstrument
 ***********************************************************************/

#if !defined(__OTCSERVER_OTCQueryInstrument_h)
#define __OTCSERVER_OTCQueryInstrument_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCQueryInstrument : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif