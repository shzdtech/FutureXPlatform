#include "XTPositionUpdated.h"
#include "XTUtility.h"
#include "XTMapping.h"
#include "XTConstant.h"
#include "../message/MessageUtility.h"
#include "XTTradeWorkerProcessor.h"

#include "../litelogger/LiteLogger.h"

#include "../dataobject/OrderDO.h"
#include "../message/DefMessageID.h"


dataobj_ptr XTPositionUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (auto pData = (CPositionDetail*)rawRespParams[1])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
		{
			auto& userId = session->getUserInfo().getUserId();

			auto position_ptr = XTUtility::ParseRawPosition(pData, userId);

			LOG_DEBUG << pData->m_strInstrumentID << ',' << pData->m_strOpenDate << ',' << pData->m_nDirection;

			pWorkerProc->UpdateSysYdPosition(userId, position_ptr);

			if (!pWorkerProc->IsLoadPositionFromDB())
			{
				if (position_ptr->ExchangeID() == EXCHANGE_SHFE)
				{
					if (pData->m_bIsToday)
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
