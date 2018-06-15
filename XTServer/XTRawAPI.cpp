#include "XTRawAPI.h"


XTRawAPI::XTRawAPI(const std::string& address)
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

void XTRawAPI::CreateApi(const std::string& address)
{
	if (!_api)
		_api = XtTraderApi::createXtTraderApi(address.data());
}

XtTraderApi * XTRawAPI::get()
{
	return _api;
}
