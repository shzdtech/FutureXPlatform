#include "CTPTradeUpdated.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"
#include "../bizutility/ContractCache.h"
#include "../databaseop/ContractDAO.h"

#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

dataobj_ptr CTPTradeUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	TradeRecordDO_Ptr ret;
	if (auto pTrade = (CThostFtdcTradeField*)rawRespParams[0])
	{
		if (ret = CTPUtility::ParseRawTrade(pTrade))
		{
			if (ret->IsSystemUserId())
			{
				ret->SetUserID(CTPUtility::MakeUserID(ret->BrokerID, ret->InvestorID));
			}

			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
			{
				if (pWorkerProc->GetUserTradeContext().InsertTrade(ret))
				{
					int multiplier = 1;
					if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(ret->InstrumentID()))
					{
						multiplier = pContractInfo->VolumeMultiple;
					}
					else
					{
						InstrumentDO instDO;
						if (ContractDAO::FindContractById(*ret, instDO))
						{
							multiplier = instDO.VolumeMultiple;
						}
					}

					auto position_ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(ret->UserID(), ret, multiplier, ret->ExchangeID() != EXCHANGE_SHFE);

					auto pProcessor = (CTPProcessor*)msgProcessor.get();
					if (pProcessor->DataLoadMask & CTPProcessor::POSITION_DATA_LOADED && position_ptr->Position() >= 0)
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
