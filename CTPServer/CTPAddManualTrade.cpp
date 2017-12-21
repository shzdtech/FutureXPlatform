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

#include "../bizutility/ManualOpHub.h"

#include <iomanip>

dataobj_ptr CTPAddManualTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	CheckLogin(session);

	auto& userInfo = session->getUserInfo();

	auto tradeDO_Ptr = std::static_pointer_cast<TradeRecordDO>(reqDO);

	auto now = std::chrono::system_clock::now();

	tradeDO_Ptr->InvestorID = userInfo.getInvestorId();
	tradeDO_Ptr->BrokerID = userInfo.getBrokerId();
	tradeDO_Ptr->SetUserID(userInfo.getUserId());
	tradeDO_Ptr->TradingDay = userInfo.getTradingDay();
	tradeDO_Ptr->TradeID = now.time_since_epoch().count();
	tradeDO_Ptr->OrderSysID = 0;
	tradeDO_Ptr->HasMore = false;

	auto time = std::chrono::system_clock::to_time_t(now);
	auto ptm = std::localtime(&time);

	char timestr[20];
	std::strftime(timestr, sizeof(timestr), "%T", ptm);
	tradeDO_Ptr->TradeTime = timestr;
	std::strftime(timestr, sizeof(timestr), "%F", ptm);
	tradeDO_Ptr->TradeDate = timestr;

	if (tradeDO_Ptr->ExchangeID().empty())
	{
		if (auto pInstument = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(tradeDO_Ptr->InstrumentID()))
			tradeDO_Ptr->setExchangeID(pInstument->ExchangeID());
	}

	TradeDAO::SaveExchangeTrade(*tradeDO_Ptr);

	ManualOpHub::Instance()->NotifyNewManualTrade(*tradeDO_Ptr);

	return nullptr;
}
