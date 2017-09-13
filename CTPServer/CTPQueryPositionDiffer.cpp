/***********************************************************************
 * Module:  CTPQueryPositionDiffer.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:54
 * Purpose: Implementation of the class CTPQueryPositionDiffer
 ***********************************************************************/

#include "CTPQueryPositionDiffer.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/UserPositionDO.h"
#include "../message/BizError.h"
#include "../message/MessageUtility.h"
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryPositionDiffer::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryPositionDiffer::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPositionDiffer::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	auto ret = std::make_shared<MapDO<std::tuple<std::string, std::string, PositionDirectionType>, std::pair<int, int>>>();

	auto& userInfo = session->getUserInfo();
	auto role = userInfo.getRole();

	if (role >= ROLE_TRADINGDESK)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			autofillmap<std::tuple<std::string, std::string, PositionDirectionType>, std::pair<int, int>> positions;
			pWorkerProc->ComparePosition(userInfo.getUserId(), positions);

			for (auto& pair : positions)
			{
				ret->emplace(pair.first, pair.second);
			}
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryPositionDiffer::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryPositionDiffer::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPositionDiffer::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}