/***********************************************************************
 * Module:  TestingQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:18:21
 * Purpose: Implementation of the class TestingQueryOrder
 ***********************************************************************/

#include "TestingQueryOrder.h"
#include "TestingWorkProcessor.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../utility/stringutility.h"

#include "../dataobject/OrderDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       TestingQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of TestingQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<TestingWorkProcessor>(msgProcessor))
	{
		auto stdo = (MapDO<std::string>*)reqDO.get();

		auto& brokeid = session->getUserInfo()->getBrokerId();
		auto& investorid = session->getUserInfo()->getInvestorId();
		auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
		auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
		auto& orderid = stdo->TryFind(STR_ORDER_ID, EMPTY_STRING);
		auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
		auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);

		auto& userOrderCtx = pWorkerProc->GetUserOrderContext();
		auto vectorPtr = userOrderCtx.GetOrdersByUser(session->getUserInfo()->getUserId());
		ThrowNotFoundExceptionIfEmpty(vectorPtr);

		if (orderid != EMPTY_STRING)
		{
			if (auto orderptr = userOrderCtx.FindOrder(std::strtoull(orderid.data(), nullptr, 0)))
			{
				auto rspOrderPtr = std::make_shared<OrderDO>(*orderptr);
				rspOrderPtr->HasMore = false;
				pWorkerProc->SendDataObject(session, MSG_ID_QUERY_ORDER, serialId, rspOrderPtr);
			}
			else
				throw NotFoundException();
		}
		else
		{
			auto lastidx = vectorPtr->size() - 1;
			for (int i = 0; i <= lastidx; i++)
			{
				auto rspOrderPtr = std::make_shared<OrderDO>(*vectorPtr->at(i));
				rspOrderPtr->HasMore = i < lastidx;
				pWorkerProc->SendDataObject(session, MSG_ID_QUERY_ORDER, serialId, rspOrderPtr);
			}
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of TestingQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}