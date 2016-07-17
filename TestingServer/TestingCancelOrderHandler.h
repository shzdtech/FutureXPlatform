#pragma once
#include "../message/NopHandler.h"

class TestingCancelOrderHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

};

