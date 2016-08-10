/***********************************************************************
 * Module:  OTCReturnPricingData.h
 * Author:  milk
 * Modified: 2015年8月25日 22:55:51
 * Purpose: Declaration of the class OTCReturnPricingData
 ***********************************************************************/

#if !defined(__OTCServer_OTCReturnPricingData_h)
#define __OTCServer_OTCReturnPricingData_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCReturnPricingData : public NopHandler
{
public:
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif