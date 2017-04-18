#include "CTPTradeUpdated.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"
#include "../bizutility/ContractCache.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

dataobj_ptr CTPTradeUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	TradeRecordDO_Ptr ret;
	if (auto pTrade = (CThostFtdcTradeField*)rawRespParams[0])
	{
		if (ret = CTPUtility::ParseRawTrade(pTrade))
		{
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
			{
				if (pWorkerProc->GetUserTradeContext().InsertTrade(ret))
				{
					PositionDirectionType pd =
						(ret->Direction == DirectionType::SELL && ret->OpenClose == OrderOpenCloseType::OPEN) ||
						(ret->Direction != DirectionType::SELL && ret->OpenClose != OrderOpenCloseType::OPEN) ?
						PositionDirectionType::PD_SHORT : PositionDirectionType::PD_LONG;

					auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(ret->InstrumentID());

					auto position_ptr = pWorkerProc->GetUserPositionContext().UpsertPosition(ret->UserID(), ret, pd, pContractInfo, ret->ExchangeID() != EXCHANGE_SHFE);

					auto pProcessor = (CTPProcessor*)msgProcessor.get();
					if (pProcessor->DataLoadMask & CTPTradeProcessor::POSITION_DATA_LOADED && position_ptr->Position() >= 0)
					{
						pWorkerProc->SendDataObject(session, MSG_ID_POSITION_UPDATED, 0, position_ptr);
					}
				}
				else
				{
					ret.reset();
				}
			}
		}
	}

	return ret;
}
