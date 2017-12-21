/***********************************************************************
 * Module:  CTPQueryPosition.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:08
 * Purpose: Implementation of the class CTPQueryPosition
 ***********************************************************************/

#include "CTPQueryPosition.h"
#include "CTPRawAPI.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

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
 // Name:       CTPQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryPosition::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();
	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		PortfolioPosition positionMap;
		auto pProcessor = (CTPProcessor*)msgProcessor.get();
		if (!(pProcessor->DataLoadMask & CTPProcessor::POSITION_DATA_LOADED))
		{
			if (CTPUtility::HasTradeInit((CTPRawAPI*)rawAPI))
			{
				CThostFtdcQryInvestorPositionField req{};
				int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryInvestorPosition(&req, serialId);
				std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
			}
		}

		auto& userId = session->getUserInfo().getUserId();
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
					pWorkerProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, position_ptr);
				}
				if (shortPosition)
				{
					found = true;
					auto position_ptr = std::make_shared<UserPositionExDO>(*shortPosition);
					pWorkerProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, position_ptr);
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
						pWorkerProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, positionDO_Ptr);

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
		CTPUtility::CheckTradeInit((CTPRawAPI*)rawAPI);

		auto& brokeid = session->getUserInfo().getBrokerId();
		auto& investorid = session->getUserInfo().getInvestorId();

		CThostFtdcQryInvestorPositionField req{};
		std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
		std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));
		std::strncpy(req.InstrumentID, instrumentid.data(), sizeof(req.InstrumentID));

		int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqQryInvestorPosition(&req, serialId);
		CTPUtility::CheckReturnError(iRet);
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryPosition::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryPosition::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPosition::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0];

	// position_ptr->HasMore = !*(bool*)rawRespParams[3];

	LOG_DEBUG << pData->InstrumentID << ',' << pData->PositionDate << ',' << pData->PosiDirection;

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		auto& userId = session->getUserInfo().getUserId();

		auto position_ptr = CTPUtility::ParseRawPosition(pData, userId);

		pWorkerProc->UpdateSysYdPosition(userId, position_ptr);

		if (!pWorkerProc->IsLoadPositionFromDB())
		{
			if (position_ptr->ExchangeID() == EXCHANGE_SHFE)
			{
				if (pData->PositionDate == THOST_FTDC_PSD_Today)
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

	return nullptr;
}