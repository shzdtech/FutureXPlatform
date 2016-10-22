#include "CTPTradeUpdated.h"
#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"

#include "../message/MessageUtility.h"

dataobj_ptr CTPTradeUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto ret = CTPUtility::ParseRawTrade((CThostFtdcTradeField*)rawRespParams[0]);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
	{
		pWorkerProc->GetUserTradeContext().UpsertTrade(ret);
	}

	return ret;
}
