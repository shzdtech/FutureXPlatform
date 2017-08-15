#pragma once
#include "CTPTradeWorkerProcessor.h"
#include "../ordermanager/PortfolioPositionContext.h"
#include "../pricingengine/IPricingDataContext.h"
#include "../utility/lockfree_queue.h"
#include <thread>
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerSAProcessor : public CTPTradeWorkerProcessor
{
public:
	CTPTradeWorkerSAProcessor(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingCtx, const IUserPositionContext_Ptr& positionCtx);
	~CTPTradeWorkerSAProcessor();

	virtual void Initialize(IServerContext* pServerCtx);

	void LogTrade();

public:
	virtual void OnRtnTrade(CThostFtdcTradeField * pTrade);

protected:
	bool _logTrades = true;
	std::thread _tradeDBSerializer;
	lockfree_queue<TradeRecordDO_Ptr> _tradeQueue;
};

