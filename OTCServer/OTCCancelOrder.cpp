/***********************************************************************
 * Module:  OTCCancelOrder.cpp
 * Author:  milk
 * Modified: 2015年10月14日 0:09:09
 * Purpose: Implementation of the class OTCCancelOrder
 ***********************************************************************/

#include "OTCCancelOrder.h"
#include <memory>

#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"
#include "../dataobject/OrderDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       OTCCancelOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of OTCCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCCancelOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto& orderDO = *((OrderRequestDO*)reqDO.get());
	
	if (auto wkProcPtr = MessageUtility::ServerWorkerProcessor<OTCWorkerProcessor>(session->getProcessor()))
	{
		wkProcPtr->GetOTCTradeProcessor()->OTCCancelOrder(orderDO);
	}

	return reqDO;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCCancelOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of OTCCancelOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCCancelOrder::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);

	return std::make_shared<OrderDO>(orderDO);
}