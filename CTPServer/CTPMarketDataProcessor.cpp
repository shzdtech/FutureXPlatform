/***********************************************************************
 * Module:  CTPMarketDataProcessor.cpp
 * Author:  milk
 * Modified: 2014年10月6日 22:17:16
 * Purpose: Implementation of the class CTPMarketDataProcessor
 ***********************************************************************/

#include <glog/logging.h>
#include "CTPMarketDataProcessor.h"
#include "CTPUtility.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"

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
	DLOG(INFO) << __FUNCTION__ << std::endl;
	if (_rawAPI.MdAPI)
		_rawAPI.MdAPI->Release();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMarketDataProcessor::OnInit()
// Purpose:    Implementation of CTPMarketDataProcessor::OnInit()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPMarketDataProcessor::Initialize(void) {
	_rawAPI.MdAPI = CThostFtdcMdApi::CreateFtdcMdApi();
	if (_rawAPI.MdAPI) {
		_rawAPI.MdAPI->RegisterSpi(this);
		std::string frontserver = _configMap[STR_FRONT_SERVER];
		_rawAPI.MdAPI->RegisterFront(const_cast<char*> (frontserver.data()));
		_rawAPI.MdAPI->Init();
	}
}

int CTPMarketDataProcessor::Login(CThostFtdcReqUserLoginField* loginInfo, uint32_t serialId)
{
	_lastLoginSerialId = serialId;
	return _rawAPI.MdAPI->ReqUserLogin(loginInfo, _lastLoginSerialId);
}

void CTPMarketDataProcessor::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast) {
	DLOG(INFO) << __FUNCTION__ << ':' << pRspInfo->ErrorMsg << std::endl;
}

void CTPMarketDataProcessor::OnFrontDisconnected(int nReason) {
	DLOG(INFO) << __FUNCTION__ << std::endl;
}

void CTPMarketDataProcessor::OnHeartBeatWarning(int nTimeLapse) {
	DLOG(INFO) << __FUNCTION__ << std::endl;
}

void CTPMarketDataProcessor::OnFrontConnected() {
	DLOG(INFO) << __FUNCTION__ << std::endl;
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