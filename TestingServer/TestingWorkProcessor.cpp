#include "TestingWorkProcessor.h"

TestingWorkProcessor::TestingWorkProcessor()
{
}

void TestingWorkProcessor::Initialize(IServerContext * pServerCtx)
{
}

IRawAPI * TestingWorkProcessor::getRawAPI(void)
{
	return nullptr;
}

UserPositionContext & TestingWorkProcessor::GetUserPositionContext()
{
	return _userPositionCtx;
}

UserTradeContext & TestingWorkProcessor::GetUserTradeContext()
{
	return _userTradeCtx;
}

UserOrderContext & TestingWorkProcessor::GetUserOrderContext(void)
{
	return _userOrderCtx;
}
