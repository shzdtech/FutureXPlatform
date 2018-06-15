/***********************************************************************
 * Module:  CTPMarketDataProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 22:17:16
 * Purpose: Implementation of the class CTPMarketDataProcessor
 ***********************************************************************/

#include "../litelogger/LiteLogger.h"
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
	if (!_rawAPI)
		_rawAPI = std::make_shared<CTPRawAPI>();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMarketDataProcessor::~CTPMarketDataProcessor()
// Purpose:    Implementation of CTPMarketDataProcessor::~CTPMarketDataProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPMarketDataProcessor::~CTPMarketDataProcessor() {
	LOG_DEBUG << __FUNCTION__;
}

bool CTPMarketDataProcessor::CreateBackendAPI(CThostFtdcMdSpi *pSpi, const std::string& flowId, const std::string & serverAddr)
{
	fs::path localpath = CTPProcessor::FlowPath;
	if (!fs::exists(localpath))
	{
		std::error_code ec;
		fs::create_directories(localpath, ec);
	}

	localpath /= flowId + "_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(std::rand()) + "_";
	std::string server_addr(serverAddr);
	if (server_addr.empty() && !_serverCtx->getConfigVal(CTP_MD_SERVER, server_addr))
	{
		SysParam::TryGet(CTP_MD_SERVER, server_addr);
	}

	auto mdAPI = std::make_shared<CTPRawAPI::CThostFtdcMdApiProxy>(localpath.string().data());
	mdAPI->get()->RegisterSpi(this);
	mdAPI->get()->RegisterFront(const_cast<char*> (server_addr.data()));
	mdAPI->get()->Init();

	std::static_pointer_cast<CTPRawAPI>(_rawAPI)->ResetMdAPIProxy(mdAPI);

	std::this_thread::sleep_for(std::chrono::seconds(2));

	return (bool)_rawAPI;
}

void CTPMarketDataProcessor::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast) {
	LOG_DEBUG << __FUNCTION__ << ": " << pRspInfo->ErrorMsg;
}

void CTPMarketDataProcessor::OnFrontConnected() {
	LOG_DEBUG << __FUNCTION__;
}

void CTPMarketDataProcessor::OnFrontDisconnected(int nReason) {
	LOG_DEBUG << __FUNCTION__;
}

void CTPMarketDataProcessor::OnHeartBeatWarning(int nTimeLapse) {
	LOG_DEBUG << __FUNCTION__;
}

void CTPMarketDataProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (!CTPUtility::HasError(pRspInfo))
	{
		_tradingDay = std::atoi(pRspUserLogin->TradingDay);
	}
	if (!nRequestID) nRequestID = LoginSerialId;
	OnResponseMacro(MSG_ID_LOGIN, nRequestID, pRspUserLogin, pRspInfo, &nRequestID, &bIsLast);
}

void CTPMarketDataProcessor::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	OnResponseMacro(MSG_ID_LOGOUT, nRequestID, pUserLogout, pRspInfo, &nRequestID, &bIsLast)
};

void CTPMarketDataProcessor::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	OnResponseMacro(MSG_ID_SUB_MARKETDATA, nRequestID, pSpecificInstrument, pRspInfo, &nRequestID, &bIsLast)
}

void CTPMarketDataProcessor::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	OnResponseMacro(MSG_ID_UNSUB_MARKETDATA, nRequestID, pSpecificInstrument, pRspInfo, &nRequestID, &bIsLast)
}

void CTPMarketDataProcessor::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
	OnResponseMacro(MSG_ID_RET_MARKETDATA, 0, pDepthMarketData)
}