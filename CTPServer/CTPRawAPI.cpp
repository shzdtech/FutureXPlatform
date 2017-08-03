#include "CTPRawAPI.h"

CTPRawAPI::CThostFtdcMdApiProxy::CThostFtdcMdApiProxy(const char * path)
{
	CreateApi(path);
}

CTPRawAPI::CThostFtdcMdApiProxy::~CThostFtdcMdApiProxy()
{
	if (_api)
	{
		_api->Release();
		_api = nullptr;
	}
}

void CTPRawAPI::CThostFtdcMdApiProxy::CreateApi(const char * path)
{
	if(!_api)
		_api = CThostFtdcMdApi::CreateFtdcMdApi(path);
}

CThostFtdcMdApi * CTPRawAPI::CThostFtdcMdApiProxy::get()
{
	return _api;
}

CTPRawAPI::CThostFtdcTdApiProxy::CThostFtdcTdApiProxy(const char * path)
{
	CreateApi(path);
}

CTPRawAPI::CThostFtdcTdApiProxy::~CThostFtdcTdApiProxy()
{
	if (_api)
	{
		_api->Release();
		_api = nullptr;
	}
}

void CTPRawAPI::CThostFtdcTdApiProxy::CreateApi(const char * path)
{
	if (!_api)
		_api = CThostFtdcTraderApi::CreateFtdcTraderApi(path);
}

CThostFtdcTraderApi * CTPRawAPI::CThostFtdcTdApiProxy::get()
{
	return _api;
}

std::shared_ptr<CTPRawAPI::CThostFtdcMdApiProxy> CTPRawAPI::MdAPIProxy()
{
	return _mdAPIProxy;
}

std::shared_ptr<CTPRawAPI::CThostFtdcTdApiProxy> CTPRawAPI::TdAPIProxy()
{
	return _tdAPIProxy;
}

void CTPRawAPI::ResetMdAPIProxy(const std::shared_ptr<CThostFtdcMdApiProxy>& proxy)
{
	_mdAPIProxy = proxy;
}

void CTPRawAPI::ResetTdAPIProxy(const std::shared_ptr<CThostFtdcTdApiProxy>& proxy)
{
	_tdAPIProxy = proxy;
}
