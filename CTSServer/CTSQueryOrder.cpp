/***********************************************************************
 * Module:  CTSQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年12月10日 12:57:18
 * Purpose: Implementation of the class CTSQueryOrder
 ***********************************************************************/

#include "CTSQueryOrder.h"
#include "CTSAPIWrapper.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSQueryOrder::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSQueryOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSQueryOrder::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSQueryOrder::HandleResponse(param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSQueryOrder::HandleResponse()
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSQueryOrder::HandleResponse(param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	OrderDO_Ptr order_ptr = *((OrderDO_Ptr*)rawParams[0]);

	return order_ptr;
}