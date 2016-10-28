#pragma once
#include "../message/NopHandler.h"

class TestingQueryInstruments : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

private:
	static bool _loaded;

};
