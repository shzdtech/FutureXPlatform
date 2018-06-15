#include "CTPSyncTrade.h"
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

#include "../bizutility/ManualOpHub.h"

#include <iomanip>

dataobj_ptr CTPSyncTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	CheckLogin(session);

	auto& userInfo = session->getUserInfo();

	auto tradeDO_Ptr = std::static_pointer_cast<TradeRecordDO>(reqDO);

	if (userInfo.getRole() >= ROLE_TRADINGDESK)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessorBase>(msgProcessor))
		{
			auto trade_ptr = pWorkerProc->GetUserTradeContext().FindTrade(tradeDO_Ptr->TradeID128());
			if (!trade_ptr)
				throw NotFoundException("Trade not found! TradeID: " + std::to_string(tradeDO_Ptr->TradeID));

			trade_ptr->SetUserID(userInfo.getUserId());
			trade_ptr->SetPortfolioID(tradeDO_Ptr->PortfolioID());

			tradeDO_Ptr = trade_ptr;
			TradeDAO::SaveExchangeTrade(*tradeDO_Ptr);

			ManualOpHub::Instance()->NotifyNewManualTrade(*tradeDO_Ptr);
		}
	}

	return tradeDO_Ptr;
}
