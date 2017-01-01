/***********************************************************************
 * Module:  CTSNewOrder.cpp
 * Author:  milk
 * Modified: 2015年12月9日 18:33:58
 * Purpose: Implementation of the class CTSNewOrder
 ***********************************************************************/

#include "CTSNewOrder.h"
#include "CTSAPIWrapper.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSNewOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTSNewOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSNewOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto pOrder = (OrderRequestDO*)reqDO.get();
	pOrder->SetUserID(session->getUserInfo().getUserId());

	auto api = (CTSAPIWrapper*)rawAPI;
	api->Impl()->CreateOrder(*pOrder);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSNewOrder::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTSNewOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSNewOrder::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	OrderDO_Ptr order_ptr = *((OrderDO_Ptr*)rawParams[0]);

	return order_ptr;
}