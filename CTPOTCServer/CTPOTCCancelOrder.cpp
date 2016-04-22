/***********************************************************************
 * Module:  CTPOTCCancelOrder.cpp
 * Author:  milk
 * Modified: 2015年10月14日 0:09:09
 * Purpose: Implementation of the class CTPOTCCancelOrder
 ***********************************************************************/

#include "CTPOTCCancelOrder.h"
#include <memory>
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/CTPAppContext.h"
#include "../strategy/PricingContext.h"
#include "CTPWorkerProcessorID.h"
#include "CTPOTCWorkerProcessor.h"
#include "../dataobject/OrderDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCCancelOrder::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCCancelOrder::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto ret = reqDO;
	auto& orderDO = *((OrderDO*)reqDO.get());
	
	if(auto wkProcPtr = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(CTPAppContext::FindServerProcessor(CTPWorkProcessorID::WORKPROCESSOR_OTC)))
	{
		wkProcPtr->OTCCancelOrder(orderDO);
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCCancelOrder::HandleResponse(ParamVector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCCancelOrder::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCCancelOrder::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);

	return std::make_shared<OrderDO>(orderDO);
}