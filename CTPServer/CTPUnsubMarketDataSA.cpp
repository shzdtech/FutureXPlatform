/***********************************************************************
 * Module:  CTPUnsubMarketDataSA.cpp
 * Author:  milk
 * Modified: 2015年3月6日 20:11:00
 * Purpose: Implementation of the class CTPUnsubMarketDataSA
 ***********************************************************************/

#include "CTPUnsubMarketDataSA.h"
#include "CTPRawAPI.h"
#include "CTPMarketDataSAProcessor.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../message/MessageUtility.h"
#include <boost/locale/encoding.hpp>
#include "CTPUtility.h"
#include <algorithm>

////////////////////////////////////////////////////////////////////////
// Name:       CTPUnsubMarketDataSA::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTPUnsubMarketDataSA::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPUnsubMarketDataSA::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPMarketDataSAProcessor>(msgProcessor))
	{
		auto stdo = (StringTableDO*)reqDO.get();
		int ret = 0;
		if (!stdo->Data.empty())
		{
			auto& instList = stdo->Data.begin()->second;
			for (auto& inst : instList)
				pWorkerProc->MarketDataNotifers->remove(inst, session);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPUnsubMarketDataSA::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTPUnsubMarketDataSA::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPUnsubMarketDataSA::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}