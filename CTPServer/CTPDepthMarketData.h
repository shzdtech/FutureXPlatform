/***********************************************************************
 * Module:  CTPDepthMarketData.h
 * Author:  milk
 * Modified: 2015年3月6日 20:17:29
 * Purpose: Declaration of the class CTPDepthMarketData
 ***********************************************************************/

#if !defined(__CTP_CTPDepthMarketData_h)
#define __CTP_CTPDepthMarketData_h

#include "../message/NopHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPDepthMarketData : public NopHandler
{
public:
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif