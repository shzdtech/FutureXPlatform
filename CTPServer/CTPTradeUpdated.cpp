#include "CTPTradeUpdated.h"
#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"

#include "../message/MessageUtility.h"

dataobj_ptr CTPTradeUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (auto pTrade = (CThostFtdcTradeField*)rawRespParams[0])
	{
		auto ret = CTPUtility::ParseRawTrade(pTrade);

		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			pWorkerProc->GetUserTradeContext().UpsertTrade(ret);
		}

		return ret;
	}

	return nullptr;
}
