#include "CTPPositionUpdated.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"
#include "../message/MessageUtility.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../litelogger/LiteLogger.h"

#include "../dataobject/OrderDO.h"
#include "../message/DefMessageID.h"
#include "tradeapi/ThostFtdcTraderApi.h"

dataobj_ptr CTPPositionUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	UserPositionExDO_Ptr ret;

	if (auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			ret = CTPUtility::ParseRawPosition(pData);

			LOG_DEBUG << pData->InstrumentID << ',' << pData->PositionDate << ',' << pData->PosiDirection;

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

			auto pProcessor = (CTPProcessor*)msgProcessor.get();
			if (!(pProcessor->DataLoadMask & CTPProcessor::POSITION_DATA_LOADED) || ret->Position() < 0)
				ret.reset();
		}
	}

	return ret;
}
