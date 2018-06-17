/***********************************************************************
 * Module:  CTPCancleOrder.cpp
 * Author:  milk
 * Modified: 2015年10月26日 11:31:17
 * Purpose: Implementation of the class CTPCancleOrder
 ***********************************************************************/

#include "XTCancelOrder.h"
#include "XTRawAPI.h"
#include "XTUtility.h"
#include "XTMapping.h"
#include "XTConstant.h"

#include "../dataobject/OrderDO.h"

#include "../message/message_macro.h"
#include "../message/DefMessageID.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPCancleOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of XTCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr XTCancelOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	XTUtility::CheckTradeInit((XTRawAPI*)rawAPI);

	auto pDO = (OrderRequestDO*)reqDO.get();

	auto& userInfo = session->getUserInfo();
	
	if (pDO->OrderSysID != 0)
	{
		char orderSysId[sizeof(uint64_t)+1];
		std::snprintf(orderSysId, sizeof(orderSysId), FMT_ORDERSYSID, pDO->OrderSysID);
		((XTRawAPI*)rawAPI)->get()->cancelOrder(userInfo.getInvestorId().data(), orderSysId, pDO->ExchangeID().data(), pDO->InstrumentID().data(), serialId);
	}
	else
	{
		((XTRawAPI*)rawAPI)->get()->cancel(pDO->OrderID, serialId);
	}


	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTCancelOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPCancleOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr XTCancelOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	XTUtility::CheckError(rawRespParams[2]);

	OrderDO_Ptr ret;

	int reqId = *(int*)rawRespParams[0];
	int orderId = *(int*)rawRespParams[1];

	return ret;

	return ret;
}