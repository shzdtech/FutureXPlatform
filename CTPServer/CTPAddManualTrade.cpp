#include "CTPAddManualTrade.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
#include "CTPTradeWorkerProcessor.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/TradeDAO.h"
#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"

dataobj_ptr CTPAddManualTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	CheckLogin(session);

	auto& userInfo = session->getUserInfo();

	auto tradeDO_Ptr = std::static_pointer_cast<TradeRecordDO>(reqDO);

	tradeDO_Ptr->InvestorID = userInfo.getInvestorId();
	tradeDO_Ptr->BrokerID = userInfo.getBrokerId();
	tradeDO_Ptr->SetUserID(userInfo.getUserId());
	tradeDO_Ptr->TradingDay = userInfo.getTradingDay();
	tradeDO_Ptr->TradeTime = std::to_string(std::time(nullptr));
	tradeDO_Ptr->TradeID = std::chrono::steady_clock::now().time_since_epoch().count();
	tradeDO_Ptr->HasMore = false;

	TradeDAO::SaveExchangeTrade(*tradeDO_Ptr);

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
	{
		pWorkerProc->GetUserTradeContext().InsertTrade(tradeDO_Ptr);

		pWorkerProc->DispatchUserMessage(MSG_ID_TRADE_RTN, 0, tradeDO_Ptr->UserID(), tradeDO_Ptr);

		int multiplier = 1;
		if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(tradeDO_Ptr->InstrumentID()))
		{
			multiplier = pContractInfo->VolumeMultiple;
		}
		else
		{
			InstrumentDO instDO;
			if (ContractDAO::FindContractById(*tradeDO_Ptr, instDO))
			{
				multiplier = instDO.VolumeMultiple;
			}
		}
		
		if (auto userPosition_Ptr = pWorkerProc->GetUserPositionContext()->UpsertPosition(tradeDO_Ptr->UserID(), tradeDO_Ptr, multiplier))
		{
			pWorkerProc->DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, tradeDO_Ptr->UserID(), userPosition_Ptr);
		}	
	}

	return nullptr;
}
