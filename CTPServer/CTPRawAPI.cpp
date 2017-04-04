#include "CTPRawAPI.h"

CTPRawAPI::~CTPRawAPI()
{
	ReleaseMdApi();
	ReleaseTdApi();
}

void CTPRawAPI::CreateMdApi(const char * path)
{
	MdAPI = CThostFtdcMdApi::CreateFtdcMdApi(path);
}

void CTPRawAPI::CreateTdApi(const char * path)
{
	TdAPI = CThostFtdcTraderApi::CreateFtdcTraderApi(path);
}

void CTPRawAPI::ReleaseMdApi()
{
	if (MdAPI)
	{
		MdAPI->Release();
		MdAPI = nullptr;
	}
}

void CTPRawAPI::ReleaseTdApi()
{
	if (TdAPI)
	{
		TdAPI->Release();
		TdAPI = nullptr;
	}
}