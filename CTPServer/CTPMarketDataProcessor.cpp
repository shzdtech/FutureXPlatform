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

int CTPMarketDataProcessor::InitializeServer(const std::string & serverAddr)
{
	int ret = 0;

	if (!_rawAPI->MdAPI) {
		_rawAPI->MdAPI = CThostFtdcMdApi::CreateFtdcMdApi();
		_rawAPI->MdAPI->RegisterSpi(this);

		std::string server_addr(serverAddr);
		if (server_addr.empty() && !_serverCtx->getConfigVal(CTP_MD_SERVER, server_addr))
		{
			server_addr = SysParam::Get(CTP_MD_SERVER);
		}

		_rawAPI->MdAPI->RegisterFront(const_cast<char*> (server_addr.data()));
		_rawAPI->MdAPI->Init();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return ret;
}

void CTPMarketDataProcessor::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast) {
	LOG_DEBUG << __FUNCTION__ << ": " << pRspInfo->ErrorMsg;
}

void CTPMarketDataProcessor::OnFrontConnected() {
	_isConnected = true;
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
	_isLogged = !CTPUtility::HasError(pRspInfo);
	if (!nRequestID) nRequestID = LoginSerialId;
	OnResponseMacro(MSG_ID_LOGIN, nRequestID, pRspUserLogin, pRspInfo, &nRequestID, &bIsLast)
}

void CTPMarketDataProcessor::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	_isLogged = false;
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