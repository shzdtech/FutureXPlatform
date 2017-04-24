#include "CTPTradeWorkerProcessorEx.h"



CTPTradeWorkerProcessorEx::CTPTradeWorkerProcessorEx(IServerContext* pServerCtx)
	: CTPTradeWorkerProcessor(pServerCtx)
{
}

IUserPositionContext & CTPTradeWorkerProcessorEx::GetUserPositionContext()
{
	return _portfolioPositionContext;
}

