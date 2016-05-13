/***********************************************************************
 * Module:  CTPOTCDepthMarketData.h
 * Author:  milk
 * Modified: 2015年8月5日 0:05:48
 * Purpose: Declaration of the class CTPOTCDepthMarketData
 ***********************************************************************/

#if !defined(__CTP_CTPOTCDepthMarketData_h)
#define __CTP_CTPOTCDepthMarketData_h

#include "../message/NopHandler.h"
#include "../pricingengine/IPricingDataContext.h"

class CTPOTCDepthMarketData : public NopHandler
{
public:
   dataobj_ptr HandleResponse(param_vector& rawRespParams, void* rawAPI, ISession* session);

protected:
	MarketDataDOMap* _mdDOMap = nullptr;

private:

};

#endif