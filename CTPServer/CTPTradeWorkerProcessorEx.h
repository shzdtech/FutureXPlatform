#pragma once
#include "CTPTradeWorkerProcessor.h"
#include "../ordermanager/PortfolioPositionContext.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerProcessorEx : public CTPTradeWorkerProcessor
{
public:
	CTPTradeWorkerProcessorEx(IServerContext* pServerCtx);

	virtual IUserPositionContext& GetUserPositionContext();
	
protected:
	PortfolioPositionContext _portfolioPositionContext;

};

