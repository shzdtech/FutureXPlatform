/***********************************************************************
 * Module:  CTPSyncPositionDiffer.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:54
 * Purpose: Implementation of the class CTPSyncPositionDiffer
 ***********************************************************************/

#include "CTPSyncPositionDiffer.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/ResultDO.h"
#include "../message/BizError.h"
#include "../message/MessageUtility.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../databaseop/PositionDAO.h"

#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"

#include "../bizutility/ContractCache.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPSyncPositionDiffer::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPSyncPositionDiffer::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSyncPositionDiffer::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto pPositionMap = (MapDO<std::tuple<std::string, std::string, PositionDirectionType>, std::pair<int, int>>*)reqDO.get();

	auto& userInfo = session->getUserInfo();
	auto role = userInfo.getRole();

	if (role >= ROLE_TRADINGDESK)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			std::vector<UserPositionDO> userPosition;

			for (auto pair : *pPositionMap)
			{
				UserPositionExDO_Ptr position_ptr;
				std::string contract, portfolio;
				PositionDirectionType direction;
				std::tie(contract, portfolio, direction) = pair.first;
				position_ptr = pWorkerProc->FindSysYdPostion(contract, portfolio, direction);
				if (!position_ptr)
				{
					if (position_ptr = pWorkerProc->FindDBYdPostion(userInfo.getUserId(), contract, portfolio, direction))
					{
						position_ptr->YdInitPosition = pair.second.second;
					}
					else
					{
						if (position_ptr = pWorkerProc->FindSysYdPostion(contract, EMPTY_STRING, direction))
						{
							if (!portfolio.empty())
							{
								if (auto dbPosition = pWorkerProc->FindDBYdPostion(userInfo.getUserId(), contract, EMPTY_STRING, direction))
								{
									dbPosition->YdInitPosition = 0;
									dbPosition = std::make_shared<UserPositionExDO>(*dbPosition);
									dbPosition->SetUserID(userInfo.getUserId());
									dbPosition->SetPortfolioID(EMPTY_STRING);
									userPosition.push_back(*dbPosition);
									ManualOpHub::Instance()->NotifyUpdateManualPosition(*dbPosition);
								}
							}
						}
					}
				}

				if (position_ptr)
				{
					position_ptr = std::make_shared<UserPositionExDO>(*position_ptr);
					position_ptr->SetUserID(userInfo.getUserId());
					position_ptr->SetPortfolioID(portfolio);
					userPosition.push_back(*position_ptr);
					ManualOpHub::Instance()->NotifyUpdateManualPosition(*position_ptr);
				}
			}

			if (!userPosition.empty())
			{
				PositionDAO::SyncPosition(pWorkerProc->GetSystemUser().getInvestorId(), userPosition);
			}
		}
	}

	return std::make_shared<ResultDO>();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPSyncPositionDiffer::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPSyncPositionDiffer::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSyncPositionDiffer::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}