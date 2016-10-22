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

#include <boolinq/boolinq.h>

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
	{
		auto stdo = (MapDO<std::string>*)reqDO.get();

		auto& brokeid = session->getUserInfo()->getBrokerId();
		auto& investorid = session->getUserInfo()->getInvestorId();
		auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
		auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
		auto& orderid = stdo->TryFind(STR_ORDER_ID, EMPTY_STRING);
		auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
		auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);

		auto vectorPtr = pWorkerProc->GetUserOrderContext().GetOrdersByUser(session->getUserInfo()->getUserId());
		if (vectorPtr->empty())
		{
			CThostFtdcQryOrderField req{};
			std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID) - 1);
			std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID) - 1);
			std::strncpy(req.InstrumentID, instrumentid.data(), sizeof(req.InstrumentID) - 1);
			std::strncpy(req.ExchangeID, exchangeid.data(), sizeof(req.ExchangeID) - 1);
			std::strncpy(req.OrderSysID, orderid.data(), sizeof(req.OrderSysID) - 1);
			std::strncpy(req.InsertTimeStart, tmstart.data(), sizeof(req.InsertTimeStart) - 1);
			std::strncpy(req.InsertTimeEnd, tmend.data(), sizeof(req.InsertTimeEnd) - 1);
			int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryOrder(&req, serialId);
			// CTPUtility::CheckReturnError(iRet);
			std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
		}

		ThrowNotFoundExceptionIfEmpty(vectorPtr.get());

		using namespace boolinq;
		if (orderid != EMPTY_STRING)
		{
			if (auto orderptr = pWorkerProc->GetUserOrderContext().FindOrder(std::strtoull(orderid.data(), nullptr, 0)))
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
			auto lastit = std::prev(vectorPtr->end());
			for (auto it = vectorPtr->begin(); it != vectorPtr->end(); it++)
			{
				auto rspOrderPtr = std::make_shared<OrderDO>(**it);
				rspOrderPtr->HasMore = it != lastit;
				pWorkerProc->SendDataObject(session, MSG_ID_QUERY_ORDER, serialId, rspOrderPtr);
			}
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);

	OrderDO_Ptr ret;
	if (auto pData = (CThostFtdcOrderField*)rawRespParams[0])
	{
		ret = CTPUtility::ParseRawOrder(pData);

		if (rawRespParams.size() > 1)
		{
			if (auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1])
				ret->ErrorCode = pRsp->ErrorID;
			ret->HasMore = !*(bool*)rawRespParams[3];

			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
			{
				if (auto order_ptr = pWorkerProc->GetUserOrderContext().FindOrder(ret->OrderSysID))
				{
					ret->SetUserID(order_ptr->UserID());
					ret->SetPortfolioID(order_ptr->PortfolioID());
				}
				pWorkerProc->GetUserOrderContext().UpsertOrder(ret->OrderSysID, *ret);
				ret.reset();
			}
		}
	}

	return ret;
}