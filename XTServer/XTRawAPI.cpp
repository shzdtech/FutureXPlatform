#include "XTRawAPI.h"


XTRawAPI::XTRawAPI(const char * address)
{
	CreateApi(address);
}

XTRawAPI::~XTRawAPI()
{
	if (_api)
	{
		_api->destroy();
		_api = nullptr;
	}
}

void XTRawAPI::CreateApi(const char * address)
{
	if (!_api)
		_api = XtTraderApi::createXtTraderApi(address);
}

XtTraderApi * XTRawAPI::get()
{
	return _api;
}
