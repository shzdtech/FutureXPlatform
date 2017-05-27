/***********************************************************************
 * Module:  CTPMarketDataProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 22:17:16
 * Purpose: Implementation of the class CTPMarketDataProcessor
 ***********************************************************************/

#include "../litelogger/LiteLogger.h"
#include <thread>
#include "CTPMarketDataProcessor.h"
#include "CTPUtility.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "CTPConstant.h"

#include <future>
#include <filesystem>

namespace fs = std::experimental::filesystem;

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPMarketDataProcessor::CTPMarketDataProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of CTPMarketDataProcessor::CTPMarketDataProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPMarketDataProcessor::CTPMarketDataProcessor()
{
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMarketDataProcessor::~CTPMarketDataProcessor()
// Purpose:    Implementation of CTPMarketDataProcessor::~CTPMarketDataProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPMarketDataProcessor::~CTPMarketDataProcessor() {
	LOG_DEBUG << __FUNCTION__;
}

bool CTPMarketDataProcessor::CreateCTPAPI(const std::string& flowId, const std::string & serverAddr)
{
	CTPRawAPI_Ptr ret = std::make_shared<CTPRawAPI>();

	fs::path localpath = CTPProcessor::FlowPath;
	if (!fs::exists(localpath))
	{
		std::error_code ec;
		fs::create_directories(localpath, ec);
	}

	localpath /= flowId + "_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(std::rand()) + "_";

	ret->CreateMdApi(localpath.string().data());
	ret->MdAPI->RegisterSpi(this);

	std::string server_addr(serverAddr);
	if (server_addr.empty() && !_serverCtx->getConfigVal(CTP_MD_SERVER, server_addr))
	{
		SysParam::TryGet(CTP_MD_SERVER, server_addr);
	}

	ret->MdAPI->RegisterFront(const_cast<char*> (server_addr.data()));
	ret->MdAPI->Init();
	_rawAPI = ret;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	return (bool)ret;
}

void CTPMarketDataProcessor::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast) {
	LOG_DEBUG << __FUNCTION__ << ": " << pRspInfo->ErrorMsg;
}

void CTPMarketDataProcessor::OnFrontConnected() {
	_isConnected = true;

	if (_isLogged)
	{
		if (_rawAPI)
		{
			if (auto pMdAPI = _rawAPI->MdAPI)
			{
				_subedInstuments.lock();
				for (auto instument : _subedInstuments.rawset())
				{
					char* contract[] = { const_cast<char*>(instument.data()) };
					pMdAPI->SubscribeMarketData(contract, 1);
				}
				_subedInstuments.unlock();
			}
		}
	}

	LOG_DEBUG << __FUNCTION__;
}

void CTPMarketDataProcessor::OnFrontDisconnected(int nReason) {
	_isConnected = false;
	LOG_DEBUG << __FUNCTION__;
}

void CTPMarketDataProcessor::OnHeartBeatWarning(int nTimeLapse) {
	LOG_DEBUG << __FUNCTION__;
}

void CTPMarketDataProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (_isLogged = !CTPUtility::HasError(pRspInfo))
	{
		_tradingDay = std::atoi(pRspUserLogin->TradingDay);
	}
	if (!nRequestID) nRequestID = LoginSerialId;
	OnResponseMacro(MSG_ID_LOGIN, nRequestID, pRspUserLogin, pRspInfo, &nRequestID, &bIsLast);
}

void CTPMarketDataProcessor::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	_isLogged = false;
	OnResponseMacro(MSG_ID_LOGOUT, nRequestID, pUserLogout, pRspInfo, &nRequestID, &bIsLast)
};

void CTPMarketDataProcessor::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pSpecificInstrument && !CTPUtility::HasError(pRspInfo))
	{
		_subedInstuments.emplace(pSpecificInstrument->InstrumentID);
	}

	OnResponseMacro(MSG_ID_SUB_MARKETDATA, nRequestID, pSpecificInstrument, pRspInfo, &nRequestID, &bIsLast)
}

void CTPMarketDataProcessor::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pSpecificInstrument)
	{
		_subedInstuments.erase(pSpecificInstrument->InstrumentID);
	}

	OnResponseMacro(MSG_ID_UNSUB_MARKETDATA, nRequestID, pSpecificInstrument, pRspInfo, &nRequestID, &bIsLast)
}

void CTPMarketDataProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
	OnResponseMacro(MSG_ID_RET_MARKETDATA, 0, pDepthMarketData)
}