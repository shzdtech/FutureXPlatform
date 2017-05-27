/***********************************************************************
 * Module:  OTCNewOrder.cpp
 * Author:  milk
 * Modified: 2015年10月14日 0:08:44
 * Purpose: Implementation of the class OTCNewOrder
 ***********************************************************************/

#include "OTCNewOrder.h"
#include <memory>

#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"
#include "../dataobject/OrderDO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCNewOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCNewOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCNewOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	OrderDO_Ptr ret;

	auto& orderDO = *((OrderRequestDO*)reqDO.get());
	orderDO.SetUserID(session->getUserInfo().getUserId());
	orderDO.TradingDay = session->getUserInfo().getTradingDay();
	if (auto pWorkerProc =
		MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		if (!(ret = pWorkerProc->GetOTCTradeProcessor()->OTCNewOrder(orderDO)))
		{
			throw BizException("Creating OTC Order failed");
		}	
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCNewOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of OTCNewOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCNewOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);
	orderDO.SerialId = serialId;

	return std::make_shared<OrderDO>(orderDO);
}