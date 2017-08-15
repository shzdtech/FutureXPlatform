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
#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"


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


	auto& userInfo = session->getUserInfo();
	auto role = userInfo.getRole();

	if (role >= ROLE_TRADINGDESK)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			pWorkerProc->SyncPosition(userInfo.getUserId());
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