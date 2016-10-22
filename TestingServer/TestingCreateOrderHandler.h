#pragma once

#include "../message/NopHandler.h"
#include <future>

class TestingCreateOrderHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
	std::future<void> _tradingTask;

private:
};

