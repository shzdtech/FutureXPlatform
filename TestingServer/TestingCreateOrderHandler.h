#pragma once

#include "../message/NopHandler.h"
#include <future>

class TestingCreateOrderHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
	std::future<void> _tradingTask;

private:
};

