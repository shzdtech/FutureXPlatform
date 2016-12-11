/***********************************************************************
 * Module:  CTPSimMarketData.h
 * Author:  milk
 * Modified: 2016年9月27日 20:17:29
 * Purpose: Declaration of the class CTPSimMarketData
 ***********************************************************************/

#if !defined(__CTP_CTPSimMarketData_h)
#define __CTP_CTPSimMarketData_h

#include "../message/NopHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPSimMarketData : public NopHandler
{
public:
	virtual dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif