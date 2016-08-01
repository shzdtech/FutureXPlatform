/***********************************************************************
 * Module:  CTPOTCReturnPricingData.h
 * Author:  milk
 * Modified: 2015年8月25日 22:55:51
 * Purpose: Declaration of the class CTPOTCReturnPricingData
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCReturnPricingData_h)
#define __CTPOTC_CTPOTCReturnPricingData_h

#include "../message/NopHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCReturnPricingData : public NopHandler
{
public:
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif