/***********************************************************************
 * Module:  CTPQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:18:21
 * Purpose: Implementation of the class CTPQueryOrder
 ***********************************************************************/

#include "CTPQueryOrder.h"
#include "CTPRawAPI.h"
#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../utility/stringutility.h"

#include "../dataobject/OrderDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
 // Purpose:    Implementation of CTPQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		auto stdo = (StringMapDO<std::string>*)reqDO.get();

		auto& brokeid = session->getUserInfo().getBrokerId();
		auto& userid = session->getUserInfo().getUserId();
		auto& investorid = session->getUserInfo().getInvestorId();
		auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
		//auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
		auto& orderid = stdo->TryFind(STR_ORDER_ID, EMPTY_STRING);
		//auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
		//auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);

		auto& userOrderCtx = pWorkerProc->GetUserOrderContext();
		auto vectorPtr = userOrderCtx.GetOrdersByUser(session->getUserInfo().getUserId());

		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		if (!(pProcessor->DataLoadMask & CTPProcessor::ORDER_DATA_LOADED))
		{
			/*CThostFtdcQryOrderField req{};
			std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
			std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));
			int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryOrder(&req, serialId);
			CTPUtility::CheckReturnError(iRet);*/

			std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
			vectorPtr = userOrderCtx.GetOrdersByUser(session->getUserInfo().getUserId());
			pProcessor->DataLoadMask |= CTPProcessor::ORDER_DATA_LOADED;
		}

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
				pWorkerProc->SendDataObject(session, MSG_ID_QUERY_ORDER, serialId, rspOrderPtr);
			}
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTPQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);

	OrderDO_Ptr ret;
	if (auto pData = (CThostFtdcOrderField*)rawRespParams[0])
	{
		if (session->getUserInfo().getUserId() == pData->UserID)
		{
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
			{
				if (auto orderid = CTPUtility::ToUInt64(pData->OrderSysID))
				{
					ret = pWorkerProc->GetUserOrderContext().FindOrder(orderid);
					if (!ret)
					{
						ret = CTPUtility::ParseRawOrder(pData);
						pWorkerProc->GetUserOrderContext().UpsertOrder(orderid, ret);
						ret.reset();
					}
				}
			}
			else
			{
				ret = CTPUtility::ParseRawOrder(pData);
				ret->HasMore = !*(bool*)rawRespParams[3];
			}
		}
	}

	return ret;
}