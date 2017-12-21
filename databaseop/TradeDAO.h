#if !defined(__databaseop_TradeDAO_h)
#define __databaseop_TradeDAO_h

#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TemplateDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS TradeDAO
{
public:
	static VectorDO_Ptr<TradeRecordDO> QueryOTCUserTrades(const std::string& userid, const ContractKey & contractKey,
		const std::string& startDt, const std::string& endDt);

	static VectorDO_Ptr<TradeRecordDO> QueryExchangeTrade(const std::string& userid, const std::string& contract,
		const std::string& startDt, const std::string& endDt);

	static VectorDO_Ptr<TradeRecordDO> QueryAllExchangeTrades(const std::string & startDt, const std::string & endDt);

	static int SaveExchangeTrade(int64_t tradeId, int64_t orderSysId,
		const std::string& exchange, const std::string& contract, int quantity, double price,
		int tradingDay, const std::string& userid, const std::string& portfolio, bool isBuy, int openclose,
		const std::string& sysuserid);

	static int SaveExchangeTrade(const TradeRecordDO& tradeDO);
};

#endif