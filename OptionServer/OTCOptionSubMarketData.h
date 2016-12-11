/***********************************************************************
 * Module:  OTCSubMarketData.h
 * Author:  milk
 * Modified: 2015年8月2日 14:14:39
 * Purpose: Declaration of the class OTCSubMarketData
 ***********************************************************************/

#if !defined(__OTCServer_OTCOptionSubMarketData_h)
#define __OTCServer_OTCOptionSubMarketData_h

#include "../message/NopHandler.h"
#include "otcoption_export.h"

class OPTION_SERVER_CLASS_EXPORT OTCOptionSubMarketData : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif