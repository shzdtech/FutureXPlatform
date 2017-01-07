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
 // Name:       CTPQueryTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryTrade::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	auto& userid = session->getUserInfo().getUserId();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		auto userTrades = pWorkerProc->GetUserTradeContext().GetTradesByUser(userid);

		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		if (!(pProcessor->DataLoadMask & CTPTradeProcessor::TRADE_DATA_LOADED))
		{
			/*CThostFtdcQryTradeField req{};
			std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
			std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));

			int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryTrade(&req, serialId);
			CTPUtility::CheckReturnError(iRet);*/

			std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
			userTrades = pWorkerProc->GetUserTradeContext().GetTradesByUser(userid);
			pProcessor->DataLoadMask |= CTPTradeProcessor::TRADE_DATA_LOADED;
		}

		if (userTrades.empty())
			throw NotFoundException();

		auto lockTb = userTrades.map()->lock_table();
		auto endit = lockTb.end();
		for (auto it = lockTb.begin(); it != endit; it++)
		{
			auto tradeptr = std::make_shared<TradeRecordDO>(*it->second);
			tradeptr->HasMore = std::next(it) != endit;
			pWorkerProc->SendDataObject(session, MSG_ID_QUERY_TRADE, serialId, tradeptr);
		}
	}
	else
	{
		auto stdo = (MapDO<std::string>*)reqDO.get();
		auto& brokeid = session->getUserInfo().getBrokerId();
		auto& investorid = session->getUserInfo().getInvestorId();
		auto& instrid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
		auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
		auto& tradeid = stdo->TryFind(STR_TRADE_ID, EMPTY_STRING);
		auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
		auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);

		CThostFtdcQryTradeField req{};
		std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
		std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));
		std::strncpy(req.InstrumentID, instrid.data(), sizeof(req.InstrumentID));
		std::strncpy(req.ExchangeID, exchangeid.data(), sizeof(req.ExchangeID));
		std::strncpy(req.TradeID, tradeid.data(), sizeof(req.TradeID));
		std::strncpy(req.TradeTimeStart, tmstart.data(), sizeof(req.TradeTimeStart));
		std::strncpy(req.TradeTimeEnd, tmend.data(), sizeof(req.TradeTimeEnd));

		int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryTrade(&req, serialId);
		CTPUtility::CheckReturnError(iRet);
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryTrade::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	auto pTradeInfo = (CThostFtdcTradeField*)rawRespParams[0];
	TradeRecordDO_Ptr ret;
	if (ret = CTPUtility::ParseRawTrade(pTradeInfo))
	{
		ret->HasMore = !*(bool*)rawRespParams[3];

		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			if (auto order_ptr = pWorkerProc->GetUserOrderContext().FindOrder(ret->OrderSysID))
			{
				ret->SetPortfolioID(order_ptr->PortfolioID());
			}

			pWorkerProc->GetUserTradeContext().InsertTrade(ret);
		}
	}

	return ret;
}