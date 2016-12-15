#include "CTPTradeUpdated.h"
#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"
#include "../bizutility/ContractCache.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

dataobj_ptr CTPTradeUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (auto pTrade = (CThostFtdcTradeField*)rawRespParams[0])
	{
		auto ret = CTPUtility::ParseRawTrade(pTrade);

		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			if (!pWorkerProc->GetUserTradeContext().Contains(ret->TradeID))
			{
				pWorkerProc->GetUserTradeContext().UpsertTrade(ret);

				PositionDirectionType pd =
					(ret->Direction == DirectionType::SELL && ret->OpenClose == OrderOpenCloseType::OPEN) ||
					(ret->Direction != DirectionType::SELL && ret->OpenClose != OrderOpenCloseType::OPEN) ?
					PositionDirectionType::PD_SHORT : PositionDirectionType::PD_LONG;

				double cost = pTrade->Price * pTrade->Volume;
				if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(ret->InstrumentID()))
					cost *= pContractInfo->VolumeMultiple;

				if (auto position_ptr = pWorkerProc->GetUserPositionContext().UpsertPosition(ret, pd, cost))
				{
					pWorkerProc->SendDataObject(session, MSG_ID_POSITION_UPDATED, 0, position_ptr);
				}
			}
		}

		return ret;
	}

	return nullptr;
}
