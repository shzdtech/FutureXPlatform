/***********************************************************************
 * Module:  CTPQueryTradeDiff.cpp
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Implementation of the class CTPQueryTradeDiff
 ***********************************************************************/

#include "CTPQueryTradeDiff.h"
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
 // Name:       CTPQueryTradeDiff::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryTradeDiff::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTradeDiff::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	auto& userid = session->getUserInfo().getUserId();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
	{
		auto userTrades = pWorkerProc->GetUserTradeContext().GetTradesByUser(userid);

		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		if (!(pProcessor->DataLoadMask & DataLoadType::TRADE_DATA_LOADED))
		{
			/*CThostFtdcQryTradeField req{};
			std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
			std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));

			int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryTrade(&req, serialId);
			CTPUtility::CheckReturnError(iRet);*/

			std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
			userTrades = pWorkerProc->GetUserTradeContext().GetTradesByUser(userid);
			pProcessor->DataLoadMask |= DataLoadType::TRADE_DATA_LOADED;
		}

		if (userTrades.empty())
			throw NotFoundException();

		std::vector<TradeRecordDO_Ptr> sendList;

		for (auto pair : userTrades.map()->lock_table())
		{
			if (pair.second->UserID() != userid && pair.second)
				sendList.push_back(std::make_shared<TradeRecordDO>(*pair.second));
		}

		if (sendList.empty())
			throw NotFoundException();

		int size = sendList.size() - 1;
		for (int i = 0; i <= size; i++)
		{
			sendList[i]->HasMore = i<size;
			pProcessor->SendDataObject(session, MSG_ID_QUERY_TRADE_DIFFER, serialId, sendList[i]);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTradeDiff::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryTradeDiff::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTradeDiff::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}