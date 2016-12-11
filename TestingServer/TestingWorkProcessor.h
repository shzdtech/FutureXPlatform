#pragma once

#include "../ordermanager/UserOrderContext.h"
#include "../ordermanager/UserTradeContext.h"
#include "../ordermanager/UserPositionContext.h"
#include "../message/TemplateMessageProcessor.h"

class TestingWorkProcessor : public TemplateMessageProcessor
{
public:
	TestingWorkProcessor();
	void Initialize(IServerContext* pServerCtx);
	virtual IRawAPI* getRawAPI(void);
	virtual UserPositionContext& GetUserPositionContext();
	virtual UserTradeContext& GetUserTradeContext();
	virtual UserOrderContext& GetUserOrderContext(void);

private:
	UserPositionContext _userPositionCtx;
	UserTradeContext _userTradeCtx;
	UserOrderContext _userOrderCtx;
};

