/***********************************************************************
 * Module:  XTQueryPosition.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:08
 * Purpose: Implementation of the class XTQueryPosition
 ***********************************************************************/

#include "XTQueryPosition.h"
#include "XTRawAPI.h"
#include "XTUtility.h"
#include "XTConstant.h"
#include "XTTradeWorkerProcessor.h"

#include "../litelogger/LiteLogger.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../common/BizErrorIDs.h"
#include "../common/Attribute_Key.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../bizutility/ContractCache.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       XTQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of XTQueryPosition::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr XTQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();
	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
	{
		PortfolioPosition positionMap;
		auto& userId = session->getUserInfo().getUserId();
		
		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		if (!(pProcessor->DataLoadMask & DataLoadType::POSITION_DATA_LOADED))
		{
			if (XTUtility::HasTradeInit((XTRawAPI*)rawAPI))
			{
				auto& investorid = session->getUserInfo().getInvestorId();
				((XTRawAPI*)rawAPI)->get()->reqPositionDetail(investorid.data(), serialId);
				std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
			}
		}
		
		positionMap = pWorkerProc->GetUserPositionContext()->GetPortfolioPositionsByUser(userId);
		if (positionMap.empty())
		{
			throw NotFoundException();
		}

		bool found = false;
		if (instrumentid != EMPTY_STRING)
		{
			for (auto it : positionMap.map()->lock_table())
			{
				UserPositionExDO_Ptr longPosition;
				it.second.map()->find(std::make_pair(instrumentid, PD_LONG), longPosition);
				UserPositionExDO_Ptr shortPosition;
				it.second.map()->find(std::make_pair(instrumentid, PD_SHORT), shortPosition);

				if (longPosition)
				{
					found = true;
					auto position_ptr = std::make_shared<UserPositionExDO>(*longPosition);
					position_ptr->HasMore = (bool)shortPosition;
					pProcessor->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, position_ptr);
				}
				if (shortPosition)
				{
					found = true;
					auto position_ptr = std::make_shared<UserPositionExDO>(*shortPosition);
					pProcessor->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, position_ptr);
				}

				if (found)
					break;
			}
		}
		else
		{
			auto locktb = positionMap.map()->lock_table();
			auto endit = locktb.end();
			for (auto it = locktb.begin(); it != endit;)
			{
				if (!it->second.empty())
				{
					auto clock = it->second.map()->lock_table();
					it++;

					auto cendit = clock.end();
					for (auto cit = clock.begin(); cit != cendit;)
					{
						auto positionDO_Ptr = std::make_shared<UserPositionExDO>(*cit->second);
						positionDO_Ptr->HasMore = ++cit != cendit && it != endit;
						pProcessor->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, positionDO_Ptr);

						found = true;
					}
				}
				else
				{
					it++;
				}
			}
		}

		if (!found)
			throw NotFoundException();
	}
	else
	{
		XTUtility::CheckTradeInit((XTRawAPI*)rawAPI);
		auto& investorid = session->getUserInfo().getInvestorId();
		((XTRawAPI*)rawAPI)->get()->reqPositionDetail(investorid.data(), serialId);
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTQueryPosition::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of XTQueryPosition::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr XTQueryPosition::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	XTUtility::CheckNotFound(rawRespParams[2]);
	XTUtility::CheckError(rawRespParams[4]);

	auto pData = (CPositionDetail*)rawRespParams[2];

	// position_ptr->HasMore = !*(bool*)rawRespParams[3];

	LOG_DEBUG << pData->m_strInstrumentID << ',' << pData->m_strOpenDate << ',' << pData->m_nDirection;

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
	{
		auto& userId = session->getUserInfo().getUserId();
		auto& sysUserId = session->getUserInfo().getInvestorId();
		auto position_ptr = XTUtility::ParseRawPosition(pData, sysUserId);

		pWorkerProc->UpdateSysYdPosition(sysUserId, position_ptr);

		if (!pWorkerProc->IsLoadPositionFromDB())
		{
			if (position_ptr->ExchangeID() == EXCHANGE_SHFE)
			{
				if (pData->m_bIsToday)
				{
					position_ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position_ptr, false, false);
				}
				else
				{
					position_ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position_ptr, true, false);
				}
			}
			else
				position_ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position_ptr, false, true);
		}
	}

	if (*(bool*)rawRespParams[3])
	{
		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		pProcessor->DataLoadMask |= DataLoadType::POSITION_DATA_LOADED;
	}

	return nullptr;
}