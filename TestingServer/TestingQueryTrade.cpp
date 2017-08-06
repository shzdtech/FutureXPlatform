/***********************************************************************
 * Module:  TestingQueryTrade.cpp
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Implementation of the class TestingQueryTrade
 ***********************************************************************/

#include "TestingQueryTrade.h"
#include "TestingWorkProcessor.h"
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

 ////////////////////////////////////////////////////////////////////////
 // Name:       TestingQueryTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of TestingQueryTrade::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingQueryTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	auto stdo = (StringMapDO<std::string>*)reqDO.get();
	auto& brokeid = session->getUserInfo().getBrokerId();
	auto& investorid = session->getUserInfo().getInvestorId();
	auto& instrid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
	auto& tradeid = stdo->TryFind(STR_TRADE_ID, EMPTY_STRING);
	auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
	auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);
	auto& userid = session->getUserInfo().getUserId();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<TestingWorkProcessor>(msgProcessor))
	{
		auto userTrades = pWorkerProc->GetUserTradeContext().GetTradesByUser(userid);

		if (userTrades.empty())
			throw NotFoundException();

		auto lockTb = userTrades.map()->lock_table();
		auto endit = lockTb.end();
		for (auto it = lockTb.begin(); it != endit;)
		{
			auto tradeptr = std::make_shared<TradeRecordDO>(*it->second);
			tradeptr->HasMore = ++it != endit;
			pWorkerProc->SendDataObject(session, MSG_ID_QUERY_TRADE, serialId, tradeptr);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of TestingQueryTrade::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}