/***********************************************************************
 * Module:  CTPOTCReturnPricingData.h
 * Author:  milk
 * Modified: 2015年8月25日 22:55:51
 * Purpose: Declaration of the class CTPOTCReturnPricingData
 ***********************************************************************/

#if !defined(__CTP_CTPOTCReturnPricingData_h)
#define __CTP_CTPOTCReturnPricingData_h

#include "../message/NopHandler.h"

class CTPOTCReturnPricingData : public NopHandler
{
public:
   dataobj_ptr HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif