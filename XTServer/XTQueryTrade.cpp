/***********************************************************************
 * Module:  XTQueryTrade.cpp
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Implementation of the class XTQueryTrade
 ***********************************************************************/

#include "XTQueryTrade.h"
#include "XTRawAPI.h"
#include "XTConstant.h"
#include "XTTradeWorkerProcessor.h"
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

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
	{
		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		if (!(pProcessor->DataLoadMask & DataLoadType::TRADE_DATA_LOADED))
		{
			if (XTUtility::HasTradeInit((XTRawAPI*)rawAPI))
			{
				auto& investorid = session->getUserInfo().getInvestorId();
				((XTRawAPI*)rawAPI)->get()->reqDealDetail(investorid.data(), serialId);

				std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
			}
		}

		auto userTrades = pWorkerProc->GetUserTradeContext().GetTradesByUser(userid);
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
			pProcessor->SendDataObject(session, MSG_ID_QUERY_TRADE, serialId, sendList[i]);
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
	XTUtility::CheckNotFound(rawRespParams[2]);
	XTUtility::CheckError(rawRespParams[4]);

	TradeRecordDO_Ptr ret;
	if (auto pTradeInfo = (CDealDetail*)rawRespParams[2])
	{
		if (ret = XTUtility::ParseRawTrade(pTradeInfo))
		{
			auto userID = session->getUserInfo().getUserId();

			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
			{
				if (auto order_ptr = pWorkerProc->GetUserOrderContext().FindOrder(ret->OrderSysID))
				{
					userID = order_ptr->UserID();
					ret->SetPortfolioID(order_ptr->PortfolioID());
				}

				pWorkerProc->GetUserTradeContext().InsertTrade(userID, ret);
				ret.reset();
			}
		}
	}

	if (*(bool*)rawRespParams[3])
	{
		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		pProcessor->DataLoadMask |= DataLoadType::TRADE_DATA_LOADED;
	}

	return ret;
}