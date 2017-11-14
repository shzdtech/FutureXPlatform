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
// Name:       OTCCancelOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of OTCCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCCancelOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto& orderDO = *((OrderRequestDO*)reqDO.get());
	
	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCTradeWorkerProcessor>(msgProcessor))
	{
		pWorkerProc->OTCCancelOrder(orderDO);
	}

	return reqDO;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCCancelOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of OTCCancelOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCCancelOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}