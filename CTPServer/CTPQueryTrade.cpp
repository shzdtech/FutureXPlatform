/***********************************************************************
 * Module:  CTPQueryTrade.cpp
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Implementation of the class CTPQueryTrade
 ***********************************************************************/

#include "CTPQueryTrade.h"
#include "CTPRawAPI.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"

#include "CTPUtility.h"
#include "CTPConstant.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTrade::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryTrade::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTrade::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	if (auto wkProcPtr =
		  MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
	{
		auto userTrades = wkProcPtr->GetUserTradeContext().GetTradesByUser(session->getUserInfo()->getUserId());
		if (userTrades->empty())
		{
			auto stdo = (MapDO<std::string>*)reqDO.get();

			auto& brokeid = session->getUserInfo()->getBrokerId();
			auto& investorid = session->getUserInfo()->getInvestorId();
			auto& instrid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
			auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
			auto& tradeid = stdo->TryFind(STR_TRADE_ID, EMPTY_STRING);
			auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
			auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);

			CThostFtdcQryTradeField req{};
			std::strcpy(req.BrokerID, brokeid.data());
			std::strcpy(req.InvestorID, investorid.data());
			std::strcpy(req.InstrumentID, instrid.data());
			std::strcpy(req.ExchangeID, exchangeid.data());
			std::strcpy(req.TradeID, tradeid.data());
			std::strcpy(req.TradeTimeStart, tmstart.data());
			std::strcpy(req.TradeTimeEnd, tmend.data());

			int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryTrade(&req, reqDO->SerialId);
			// CTPUtility::CheckReturnError(iRet);
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}

		ThrowNotFoundExceptionIfEmpty(userTrades);

		auto lastit = std::prev(userTrades->end());
		for (auto it = userTrades->begin(); it != userTrades->end(); it++)
		{
			auto tradeptr = std::make_shared<TradeRecordDO>(*it);
			tradeptr->HasMore = it != lastit;
			wkProcPtr->SendDataObject(session, MSG_ID_QUERY_TRADE, reqDO->SerialId, tradeptr);
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryTrade::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	auto pTradeInfo = (CThostFtdcTradeField*)rawRespParams[0];
	TradeRecordDO_Ptr ret;
	if (ret = CTPUtility::ParseRawTrade(pTradeInfo))
	{
		ret->HasMore = !*(bool*)rawRespParams[3];

		if (auto wkProcPtr =
			  MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
		{
			if (auto order_ptr = wkProcPtr->GetUserOrderContext().FindOrder(ret->OrderID))
			{
				ret->SetUserID(order_ptr->PortfolioID());
				ret->SetPortfolioID(order_ptr->PortfolioID());
			}

			wkProcPtr->GetUserTradeContext().AddTrade(*ret);
		}
	}

	return ret;
}