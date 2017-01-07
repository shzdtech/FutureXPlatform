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
	TrdAPI = CThostFtdcTraderApi::CreateFtdcTraderApi(path);
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
	if (TrdAPI)
	{
		TrdAPI->Release();
		TrdAPI = nullptr;
	}
}