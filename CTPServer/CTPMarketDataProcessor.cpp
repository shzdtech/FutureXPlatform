/***********************************************************************
 * Module:  CTPMarketDataProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 22:17:16
 * Purpose: Implementation of the class CTPMarketDataProcessor
 ***********************************************************************/

#include "../utility/LiteLogger.h"
#include <thread>
#include "CTPMarketDataProcessor.h"
#include "CTPUtility.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"

#include <future>

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPMarketDataProcessor::CTPMarketDataProcessor(const std::map<std::string, std::string>& configMap)
 // Purpose:    Implementation of CTPMarketDataProcessor::CTPMarketDataProcessor()
 // Parameters:
 // - frontserver
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPMarketDataProcessor::CTPMarketDataProcessor(const std::map<std::string, std::string>& configMap)
	: CTPProcessor(configMap) {
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMarketDataProcessor::~CTPMarketDataProcessor()
// Purpose:    Implementation of CTPMarketDataProcessor::~CTPMarketDataProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPMarketDataProcessor::~CTPMarketDataProcessor() {
	LOG_DEBUG << __FUNCTION__;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMarketDataProcessor::OnInit()
// Purpose:    Implementation of CTPMarketDataProcessor::OnInit()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPMarketDataProcessor::Initialize(void) {
	if (!_rawAPI->MdAPI) {
		_rawAPI->MdAPI = CThostFtdcMdApi::CreateFtdcMdApi();
		_rawAPI->MdAPI->RegisterSpi(this);
		_rawAPI->MdAPI->RegisterFront(const_cast<char*> (_configMap[STR_FRONT_SERVER].data()));
		_rawAPI->MdAPI->Init();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int CTPMarketDataProcessor::Login(CThostFtdcReqUserLoginField* loginInfo, uint32_t serialId)
{
	_lastLoginSerialId = serialId;
	return _rawAPI->MdAPI->ReqUserLogin(loginInfo, _lastLoginSerialId);
}

void CTPMarketDataProcessor::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast) {
	LOG_DEBUG << __FUNCTION__ << ": " << pRspInfo->ErrorMsg << '\n';
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
	OnResponseMacro(MSG_ID_LOGIN, _lastLoginSerialId,
		pRspUserLogin, pRspInfo, &nRequestID, &bIsLast)
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