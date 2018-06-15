/***********************************************************************
 * Module:  CTPQueryPositionSA.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:08
 * Purpose: Implementation of the class CTPQueryPositionSA
 ***********************************************************************/

#include "CTPQueryPositionSA.h"
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
 // Name:       CTPQueryPositionSA::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryPositionSA::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPositionSA::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();
	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
	{
		PortfolioPosition positionMap;

		auto& userId = session->getUserInfo().getUserId();
		positionMap = pWorkerProc->GetUserPositionContext()->GetPortfolioPositionsByUser(userId);
		if (positionMap.empty())
		{
			throw NotFoundException();
		}

		auto pTemplateProc = (TemplateMessageProcessor*)msgProcessor.get();

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
					pTemplateProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, position_ptr);
				}
				if (shortPosition)
				{
					found = true;
					auto position_ptr = std::make_shared<UserPositionExDO>(*shortPosition);
					pTemplateProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, position_ptr);
				}

				if (found)
					break;
			}
		}
		else
		{
			auto locktb = positionMap.map()->lock_table();
			for (auto it = locktb.begin(); it != locktb.end();)
			{
				if (!it->second.empty())
				{
					auto clock = it->second.map()->lock_table();

					++it;
					for (auto cit = clock.begin(); cit != clock.end(); )
					{
						auto positionDO_Ptr = std::make_shared<UserPositionExDO>(*cit->second);
						positionDO_Ptr->HasMore = ++cit != clock.end() && it != locktb.end();
						pTemplateProc->SendDataObject(session, MSG_ID_QUERY_POSITION, serialId, positionDO_Ptr);

						found = true;
					}
				}
				else
				{
					++it;
				}
			}
		}

		if (!found)
			throw NotFoundException();

		//if (!pWorkerProc->IsLoadPositionFromDB())
		//	pWorkerProc->QueryUserPositionAsyncIfNeed();
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryPositionSA::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryPositionSA::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPositionSA::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}