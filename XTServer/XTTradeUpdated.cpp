#include "XTTradeUpdated.h"
#include "XTUtility.h"
#include "XTConstant.h"
#include "XTTradeWorkerProcessor.h"
#include "../bizutility/ContractCache.h"
#include "../databaseop/ContractDAO.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

dataobj_ptr XTTradeUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	TradeRecordDO_Ptr ret;
	if (auto pTrade = (CDealDetail*)rawRespParams[0])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<XTTradeWorkerProcessor>(msgProcessor))
		{
			auto& userId = session->getUserInfo().getUserId();

			if (ret = pWorkerProc->RefineTrade(pTrade))
			{
				pWorkerProc->GetUserTradeContext().InsertTrade(session->getUserInfo().getUserId(), ret);

				if (!pWorkerProc->GetUserPositionContext()->ContainsTrade(ret->TradeID128()))
				{
					int multiplier = 1;
					if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(ret->InstrumentID()))
					{
						multiplier = pContractInfo->VolumeMultiple;
					}

					if (auto position_ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, ret, multiplier, ret->ExchangeID() != EXCHANGE_SHFE))
					{
						pWorkerProc->PushToLogQueue(ret);

						auto pTemplateProcessor = (TemplateMessageProcessor*)msgProcessor.get();
						if (position_ptr->Position() >= 0)
						{
							pTemplateProcessor->SendDataObject(session, MSG_ID_POSITION_UPDATED, 0, position_ptr);
						}
					}
				}

				//if (userId != pTrade->UserID)
				//{
				//	ret.reset();
				//}
			}
		}
	}

	return ret;
}
