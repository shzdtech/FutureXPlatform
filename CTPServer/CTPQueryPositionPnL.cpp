/***********************************************************************
 * Module:  CTPQueryPositionPnL.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:38:08
 * Purpose: Implementation of the class CTPQueryPositionPnL
 ***********************************************************************/

#include "CTPQueryPositionPnL.h"
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
 // Name:       CTPQueryPositionPnL::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryPositionPnL::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPositionPnL::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();
	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		auto& userId = session->getUserInfo().getUserId();
		auto positionPnLMap = pWorkerProc->GetUserPositionContext()->GetPortfolioPositionsPnLByUser(userId);
		if (positionPnLMap.empty())
		{
			throw NotFoundException();
		}

		bool found = false;

		auto locktb = positionPnLMap.map()->lock_table();
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
					auto positionPnLDO_Ptr = std::make_shared<PositionPnLDO>(*cit->second);
					positionPnLDO_Ptr->HasMore = ++cit != cendit && it != endit;
					pWorkerProc->SendDataObject(session, MSG_ID_QUERY_POSITIONPNL, serialId, positionPnLDO_Ptr);

					found = true;
				}
			}
			else
			{
				it++;
			}
		}

		if (!found)
			throw NotFoundException();
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryPositionPnL::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryPositionPnL::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryPositionPnL::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}