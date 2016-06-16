/***********************************************************************
 * Module:  CTPTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Implementation of the class CTPTradeWorkerProcessor
 ***********************************************************************/

#include "CTPTradeWorkerProcessor.h"
#include "CTPUtility.h"
#include "CTPConstant.h"

#include "../common/Attribute_Key.h"
#include "../message/AppContext.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Purpose:    Implementation of CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::CTPTradeWorkerProcessor(const std::map<std::string, std::string>& configMap)
	: CTPTradeProcessor(configMap)
{
	InstrmentsLoaded = false;
	_defaultUser.setBrokerId(SysParam::Get(CTP_TRADER_BROKERID));
	_defaultUser.setUserId(SysParam::Get(CTP_TRADER_USERID));
	_defaultUser.setPassword(SysParam::Get(CTP_TRADER_PASSWORD));
	_defaultUser.setServer(SysParam::Get(CTP_TRADER_SERVER));
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Purpose:    Implementation of CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::OnInit()
// Purpose:    Implementation of CTPTradeWorkerProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPTradeWorkerProcessor::Initialize(void)
{
	_rawAPI.TrdAPI = CThostFtdcTraderApi::CreateFtdcTraderApi();
	if (_rawAPI.TrdAPI) {
		_rawAPI.TrdAPI->RegisterSpi(this);
		_rawAPI.TrdAPI->RegisterFront(const_cast<char*> (_defaultUser.getServer().data()));
		_rawAPI.TrdAPI->SubscribePrivateTopic(THOST_TERT_RESUME);
		_rawAPI.TrdAPI->SubscribePublicTopic(THOST_TERT_RESUME);
		_rawAPI.TrdAPI->Init();
	}

	_initializer = std::async(std::launch::async, [this]() {
		std::this_thread::sleep_for(std::chrono::seconds(std::rand() % 10)); //wait some secs for connecting to CTP server
		LoginDefault();
		while (!this->InstrmentsLoaded)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(this->RetryInterval));
			LoginDefault();
		}
	});
}

int CTPTradeWorkerProcessor::LoginDefault(void)
{
	CThostFtdcReqUserLoginField req;
	std::memset(&req, 0, sizeof(req));
	std::strcpy(req.BrokerID, _defaultUser.getBrokerId().data());
	std::strcpy(req.UserID, _defaultUser.getUserId().data());
	std::strcpy(req.Password, _defaultUser.getPassword().data());
	return _rawAPI.TrdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
}

int CTPTradeWorkerProcessor::LoginIfNeed(void)
{
	int ret = 0;

	if (!_isLogged)
		ret = LoginDefault();

	return ret;
}

///登录请求响应
void CTPTradeWorkerProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!CTPUtility::HasError(pRspInfo))
	{
		_isLogged = true;

		if (!InstrmentsLoaded)
		{
			CThostFtdcQryInstrumentField req;
			std::memset(&req, 0x0, sizeof(CThostFtdcQryInstrumentField));
			_rawAPI.TrdAPI->ReqQryInstrument(&req, 0);
		}
	}
}

void CTPTradeWorkerProcessor::OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	try
	{
		InstrmentsLoaded = bIsLast;
		ProcessResponseMacro(this, MSG_ID_QUERY_INSTRUMENT, nRequestID, pInstrument, pRspInfo, &nRequestID, &bIsLast);
	}
	catch (...) {}
}
