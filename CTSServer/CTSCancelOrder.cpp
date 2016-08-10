/***********************************************************************
 * Module:  CTSCancelOrder.cpp
 * Author:  milk
 * Modified: 2015年12月10日 12:56:23
 * Purpose: Implementation of the class CTSCancelOrder
 ***********************************************************************/

#include "CTSCancelOrder.h"
#include "CTSAPIWrapper.h"
#include "../message/BizError.h"
#include "../common/BizErrorIDs.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSCancelOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSCancelOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto pOrder = (OrderRequestDO*)reqDO.get();
	auto api = (CTSAPIWrapper*)rawAPI;
	api->Impl()->CancelOrder(*pOrder);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSCancelOrder::HandleResponse(const uint32_t serialId, param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSCancelOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSCancelOrder::HandleResponse(const uint32_t serialId, param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	OrderDO_Ptr order_ptr = *((OrderDO_Ptr*)rawParams[0]);

	return order_ptr;
}