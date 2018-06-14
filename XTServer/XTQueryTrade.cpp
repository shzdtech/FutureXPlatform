/***********************************************************************
 * Module:  XTQueryTrade.cpp
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Implementation of the class XTQueryTrade
 ***********************************************************************/

#include "XTQueryTrade.h"
#include "XTRawAPI.h"
#include "CTPConstant.h"
#include "XTTradeWorkerProcessor.h"
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

#include "XTUtility.h"
#include "CTPConstant.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       XTQueryTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of XTQueryTrade::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr XTQueryTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	auto& userid = session->getUserInfo().getUserId();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		auto userTrades = pWorkerProc->GetUserTradeContext().GetTradesByUser(userid);

		auto pProcessor = (XTProcessor*)msgProcessor.get();
		if (!(pProcessor->DataLoadMask & DataLoadType::TRADE_DATA_LOADED))
		{
			/*CThostFtdcQryTradeField req{};
			std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
			std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));

			int iRet = ((XTRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryTrade(&req, serialId);
			XTUtility::CheckReturnError(iRet);*/

			std::this_thread::sleep_for(XTProcessor::DefaultQueryTime);
			userTrades = pWorkerProc->GetUserTradeContext().GetTradesByUser(userid);
			pProcessor->DataLoadMask |= DataLoadType::TRADE_DATA_LOADED;
		}

		if (userTrades.empty())
			throw NotFoundException();

		std::vector<TradeRecordDO_Ptr> sendList;

		for (auto pair : userTrades.map()->lock_table())
		{
			if (pair.second->UserID() == userid && pair.second)
				sendList.push_back(std::make_shared<TradeRecordDO>(*pair.second));
		}

		
		if (sendList.empty())
			throw NotFoundException();

		int size = sendList.size() - 1;
		for(int i=0; i<=size; i++)
		{
			sendList[i]->HasMore = i<size;
			pWorkerProc->SendDataObject(session, MSG_ID_QUERY_TRADE, serialId, sendList[i]);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of XTQueryTrade::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr XTQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	XTUtility::CheckNotFound(rawRespParams[0]);
	XTUtility::CheckError(rawRespParams[1]);

	auto pTradeInfo = (CThostFtdcTradeField*)rawRespParams[0];
	TradeRecordDO_Ptr ret;
	if (ret = XTUtility::ParseRawTrade(pTradeInfo))
	{
		auto userID = session->getUserInfo().getUserId();
		ret->HasMore = !*(bool*)rawRespParams[3];

		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			if (auto order_ptr = pWorkerProc->GetUserOrderContext().FindOrder(ret->OrderSysID))
			{
				userID = order_ptr->UserID();
				ret->SetPortfolioID(order_ptr->PortfolioID());
			}

			pWorkerProc->GetUserTradeContext().InsertTrade(userID, ret);
		}
	}

	return ret;
}