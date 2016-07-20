#pragma once

#include "../message/NopHandler.h"

class TestingAccountInfoHandler : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
};

