/***********************************************************************
 * Module:  CTPOTCDepthMarketData.h
 * Author:  milk
 * Modified: 2015年8月5日 0:05:48
 * Purpose: Declaration of the class CTPOTCDepthMarketData
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCDepthMarketData_h)
#define __CTPOTC_CTPOTCDepthMarketData_h

#include "../message/NopHandler.h"
#include "../pricingengine/IPricingDataContext.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCDepthMarketData : public NopHandler
{
public:
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, void* rawAPI, ISession* session);

protected:
	MarketDataDOMap* _mdDOMap = nullptr;

private:

};

#endif