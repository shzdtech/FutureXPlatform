/***********************************************************************
 * Module:  XTQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:18:21
 * Purpose: Implementation of the class XTQueryOrder
 ***********************************************************************/

#include "XTQueryOrder.h"
#include "XTRawAPI.h"
#include "XTUtility.h"
#include "XTTradeWorkerProcessor.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../utility/TUtil.h"
#include "../utility/stringutility.h"

#include "../dataobject/OrderDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       XTQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
 // Purpose:    Implementation of XTQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr XTQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<XTTradeWorkerProcessor>(msgProcessor))
	{
		auto stdo = (StringMapDO<std::string>*)reqDO.get();

		auto& userid = session->getUserInfo().getUserId();
		auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
		//auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
		auto& orderid = stdo->TryFind(STR_ORDER_ID, EMPTY_STRING);
		//auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
		//auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);

		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		if (!(pProcessor->DataLoadMask & DataLoadType::ORDER_DATA_LOADED))
		{
			if (XTUtility::HasTradeInit((XTRawAPI*)rawAPI))
			{
				auto& investorid = session->getUserInfo().getInvestorId();
				((XTRawAPI*)rawAPI)->get()->reqOrderDetail(investorid.data(), serialId);

				std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
			}
		}

		auto& userOrderCtx = pWorkerProc->GetUserOrderContext();
		auto vectorPtr = userOrderCtx.GetOrdersByUser(session->getUserInfo().getUserId());

		ThrowNotFoundExceptionIfEmpty(vectorPtr);

		if (orderid != EMPTY_STRING)
		{
			if (auto orderptr = userOrderCtx.FindOrder(std::strtoull(orderid.data(), nullptr, 0)))
			{
				auto rspOrderPtr = std::make_shared<OrderDO>(*orderptr);
				rspOrderPtr->HasMore = false;
				pProcessor->SendDataObject(session, MSG_ID_QUERY_ORDER, serialId, rspOrderPtr);
			}
			else
				throw NotFoundException();
		}
		else
		{
			std::vector<OrderDO_Ptr> orderList;
			for (auto& order : orderList)
			{
				auto rspOrderPtr = std::make_shared<OrderDO>(*order);
				if (userid == order->UserID())
					orderList.push_back(order);
			}

			if (orderList.empty())
				throw NotFoundException();

			auto lastidx = orderList.size() - 1;
			for (int i = 0; i <= lastidx; i++)
			{
				auto rspOrderPtr = std::make_shared<OrderDO>(*orderList[i]);
				rspOrderPtr->HasMore = i < lastidx;
				pProcessor->SendDataObject(session, MSG_ID_QUERY_ORDER, serialId, rspOrderPtr);
			}
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of XTQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr XTQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	XTUtility::CheckNotFound(rawRespParams[2]);

	OrderDO_Ptr ret;
	if (auto pData = (COrderDetail*)rawRespParams[2])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
		{
			if (auto orderid = XTUtility::ToUInt64(pData->m_strOrderSysID))
			{
				ret = pWorkerProc->GetUserOrderContext().FindOrder(orderid);
				if (!ret)
				{
					ret = XTUtility::ParseRawOrder(pData);
					pWorkerProc->GetUserOrderContext().UpsertOrder(orderid, ret);
					ret.reset();
				}
			}
		}
		else
		{
			ret = XTUtility::ParseRawOrder(pData);
			ret->HasMore = !*(bool*)rawRespParams[3];
		}

		
	}

	if (*(bool*)rawRespParams[3])
	{
		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		pProcessor->DataLoadMask |= DataLoadType::ORDER_DATA_LOADED;
	}

	return ret;
}