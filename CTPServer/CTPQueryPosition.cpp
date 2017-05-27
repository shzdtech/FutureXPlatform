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
		if (!(pProcessor->DataLoadMask & CTPTradeProcessor::POSITION_DATA_LOADED))
		{
			CThostFtdcQryInvestorPositionField req{};
			int iRet = ((CTPRawAPI*)rawAPI)->TdAPI->ReqQryInvestorPosition(&req, serialId);
			CTPUtility::CheckReturnError(iRet);
			std::this_thread::sleep_for(CTPProcessor::DefaultQueryTime);
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
			for (auto it = locktb.begin(); it != endit; it++)
			{
				if (!it->second.empty())
				{
					auto clock = it->second.map()->lock_table();
					auto cendit = clock.end();
					for (auto cit = clock.begin(); cit != cendit; cit++)
					{
						auto positionDO_Ptr = std::make_shared<UserPositionExDO>(*cit->second);
						positionDO_Ptr->HasMore = std::next(it) != endit && std::next(cit) != cendit;
						pWorkerProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, positionDO_Ptr);

						found = true;
					}
				}
			}
		}

		if (!found)
			throw NotFoundException();
	}
	else
	{
		auto& brokeid = session->getUserInfo().getBrokerId();
		auto& investorid = session->getUserInfo().getInvestorId();

		CThostFtdcQryInvestorPositionField req{};
		std::strncpy(req.BrokerID, brokeid.data(), sizeof(req.BrokerID));
		std::strncpy(req.InvestorID, investorid.data(), sizeof(req.InvestorID));
		std::strncpy(req.InstrumentID, instrumentid.data(), sizeof(req.InstrumentID));

		int iRet = ((CTPRawAPI*)rawAPI)->TdAPI->ReqQryInvestorPosition(&req, serialId);
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

	auto ret = CTPUtility::ParseRawPosition(pData);

	ret->HasMore = !*(bool*)rawRespParams[3];

	LOG_DEBUG << pData->InstrumentID << ',' << pData->PositionDate << ',' << pData->PosiDirection;

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		if (ret->ExchangeID() == EXCHANGE_SHFE)
		{
			if (pData->PositionDate == THOST_FTDC_PSD_Today)
			{
				ret = pWorkerProc->GetUserPositionContext()->UpsertPosition(session->getUserInfo().getUserId(), *ret, false, false);
			}
			else
			{
				ret = pWorkerProc->GetUserPositionContext()->UpsertPosition(session->getUserInfo().getUserId(), *ret, true, false);
			}
		}
		else
			ret = pWorkerProc->GetUserPositionContext()->UpsertPosition(session->getUserInfo().getUserId(), *ret, false, true);
	}

	return nullptr;
}