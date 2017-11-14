#pragma once
#include "CTPTradeWorkerProcessor.h"
#include "../ordermanager/PortfolioPositionContext.h"
#include "../pricingengine/IPricingDataContext.h"
#include <thread>
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerSAProcessor : public CTPTradeWorkerProcessor
{
public:
	CTPTradeWorkerSAProcessor(IServerContext* pServerCtx, const IPricingDataContext_Ptr& pricingCtx, const IUserPositionContext_Ptr& positionCtx);
	~CTPTradeWorkerSAProcessor();

	virtual void Initialize(IServerContext* pServerCtx);

public:
	virtual void OnRtnTrade(CThostFtdcTradeField * pTrade);

protected:

};

