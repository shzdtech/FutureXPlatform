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
	virtual dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif