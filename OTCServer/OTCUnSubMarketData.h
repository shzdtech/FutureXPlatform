/***********************************************************************
 * Module:  CTPOCTUnSubMarketData.h
 * Author:  milk
 * Modified: 2016年4月19日 12:59:04
 * Purpose: Declaration of the class CTPOCTUnSubMarketData
 ***********************************************************************/

#if !defined(__OTCServer_OCTUnSubMarketData_h)
#define __OTCServer_OCTUnSubMarketData_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCUnSubMarketData : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif