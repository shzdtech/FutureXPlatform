/***********************************************************************
 * Module:  CTPOTCNewOrder.cpp
 * Author:  milk
 * Modified: 2015年10月14日 0:08:44
 * Purpose: Implementation of the class CTPOTCNewOrder
 ***********************************************************************/

#include "CTPOTCNewOrder.h"
#include <memory>
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/CTPAppContext.h"
#include "../strategy/PricingContext.h"
#include "CTPWorkerProcessorID.h"
#include "CTPOTCWorkerProcessor.h"
#include "../dataobject/OrderDO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCNewOrder::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPOTCNewOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCNewOrder::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	dataobj_ptr ret = reqDO;
	auto& orderDO = *((OrderDO*)reqDO.get());
	orderDO.SetUserID(session->getUserInfo()->getUserId());
	if (auto wkProcPtr = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(CTPAppContext::FindServerProcessor(CTPWorkProcessorID::WORKPROCESSOR_OTC)))
	{
		int rtn = wkProcPtr->OTCNewOrder(orderDO);
		if (rtn != 0)
			throw BizError(rtn, "Creating OTC Order failed");
			
		wkProcPtr->RegisterOTCOrderListener(orderDO.OrderID, (IMessageSession*)session);
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCNewOrder::HandleResponse(ParamVector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCNewOrder::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCNewOrder::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);

	return std::make_shared<OrderDO>(orderDO);
}