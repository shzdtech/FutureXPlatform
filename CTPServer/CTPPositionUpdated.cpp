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
#include "CTPAPISwitch.h"

dataobj_ptr CTPPositionUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			auto& userId = session->getUserInfo().getUserId();

			auto position_ptr = CTPUtility::ParseRawPosition(pData, userId);

			LOG_DEBUG << pData->InstrumentID << ',' << pData->PositionDate << ',' << pData->PosiDirection;

			pWorkerProc->UpdateSysYdPosition(userId, position_ptr);

			if (!pWorkerProc->IsLoadPositionFromDB())
			{
				if (position_ptr->ExchangeID() == EXCHANGE_SHFE)
				{
					if (pData->PositionDate == THOST_FTDC_PSD_Today)
					{
						position_ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position_ptr, false, false);
					}
					else
					{
						position_ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position_ptr, true, false);
					}
				}
				else
				{
					position_ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position_ptr, false, true);
				}

				if (position_ptr)
					pWorkerProc->DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, userId, position_ptr);
			}
		}
	}

	return nullptr;
}
