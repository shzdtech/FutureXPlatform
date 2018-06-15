/***********************************************************************
 * Module:  XTTradeProcessor.cpp
 * Author:  milk
 * Modified: 2015年3月8日 11:35:24
 * Purpose: Implementation of the class XTTradeProcessor
 ***********************************************************************/

#include "XTTradeProcessor.h"
#include "XTUtility.h"
#include "XTConstant.h"
#include <thread>
#include "../litelogger/LiteLogger.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../bizutility/ContractCache.h"
#include "../message/MessageUtility.h"
#include "XTTradeWorkerProcessor.h"

#include <filesystem>

namespace fs = std::experimental::filesystem;
////////////////////////////////////////////////////////////////////////
// Name:       XTTradeProcessor::XTTradeProcessor(const std::map<std::string, std::string>& configMap)
// Purpose:    Implementation of XTTradeProcessor::XTTradeProcessor()
// Parameters:
// - frontserver
// Return:     
////////////////////////////////////////////////////////////////////////

XTTradeProcessor::XTTradeProcessor()
{
	_exiting = false;
	_tradeCnt = 0;
}

XTTradeProcessor::XTTradeProcessor(const XTRawAPI_Ptr& rawAPI)
	: CTPProcessor(rawAPI)
{
	_exiting = false;
	_tradeCnt = 0;
}


////////////////////////////////////////////////////////////////////////
// Name:       XTTradeProcessor::~XTTradeProcessor()
// Purpose:    Implementation of XTTradeProcessor::~XTTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

XTTradeProcessor::~XTTradeProcessor()
{
	LOG_DEBUG << __FUNCTION__;

	_exiting = true;
	if (_updateTask.valid())
		_updateTask.wait_for(std::chrono::seconds(3));
}


bool XTTradeProcessor::CreateBackendAPI(XtTraderApiCallback *pSpi, const std::string& flowId, const std::string & serverAddr)
{
	while (_updateFlag.test_and_set(std::memory_order::memory_order_acquire));

	auto tdAPI = std::make_shared<XTRawAPI>(serverAddr.data());
	tdAPI->get()->setCallback(pSpi);
	tdAPI->get()->init();

	_rawAPI = tdAPI;

	std::this_thread::sleep_for(std::chrono::seconds(2));

	_updateFlag.clear(std::memory_order::memory_order_release);

	return (bool)_rawAPI;
}

bool XTTradeProcessor::OnSessionClosing(void)
{
	_exiting = true;
	if (_updateTask.valid())
		_updateTask.wait_for(std::chrono::seconds(3));

	if (auto sessionptr = getMessageSession())
	{
		if (auto tdProxy = TradeApi())
		{
			tdProxy->get()->userLogout(sessionptr->getUserInfo().getInvestorId().data(), sessionptr->getUserInfo().getPassword().data(), 0);
		}
	}

	return true;
}

void XTTradeProcessor::QueryPositionAsync(void)
{
	while (!_exiting)
	{
		uint currentCnt = _tradeCnt;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		if (!_exiting && currentCnt == _tradeCnt)
		{
			if (auto tdProxy = TradeApi())
			{
				CThostFtdcQryInvestorPositionField req{};
				_updateFlag.clear(std::memory_order::memory_order_release);
				tdProxy->get()->reqPositionDetail(getMessageSession()->getUserInfo().getInvestorId().data(), -1);
				_updateFlag.test_and_set(std::memory_order::memory_order_acquire); // check if lock
				break;
			}
		}
	}
}

XTRawAPI_Ptr & XTTradeProcessor::TradeApi()
{
	return std::static_pointer_cast<XTRawAPI>(_rawAPI);
}


void XTTradeProcessor::QueryUserPositionAsyncIfNeed()
{
	// Try update position
	if (!_exiting && !_updateFlag.test_and_set(std::memory_order::memory_order_acquire))
	{
		_updateTask = std::async(std::launch::async, &XTTradeProcessor::QueryPositionAsync, this);
	}
}


// XT Callback API
void XTTradeProcessor::onConnected(bool success, const char* errorMsg)
{
}

///登录请求响应
void XTTradeProcessor::onUserLogin(const char* userName, const char* password, int nRequestId, const XtError& error)
{
	if (!nRequestId) nRequestId = LoginSerialId;
	OnResponseMacro(MSG_ID_LOGIN, nRequestId, userName, password, &nRequestId, &error)
}

///登出请求响应
void XTTradeProcessor::onUserLogout(const char* userName, const char* password, int nRequestId, const XtError& error)
{
	if (!_exiting)
		OnResponseMacro(MSG_ID_LOGOUT, nRequestId, userName, password, &nRequestId, &error)
}

///报单录入请求响应
void XTTradeProcessor::onOrder(int nRequestId, int orderID, const XtError& error)
{
	OnResponseMacro(MSG_ID_ORDER_NEW, nRequestId, &nRequestId, &orderID, &error)
}

///报单操作请求响应
void XTTradeProcessor::onCancelOrder(int nRequestId, const XtError& error)
{
	OnResponseMacro(MSG_ID_ORDER_CANCEL, nRequestId, &nRequestId, &error)
}

///请求查询报单响应
void XTTradeProcessor::onReqOrderDetail(const char* accountID, int nRequestId, const COrderDetail* data, bool isLast, const XtError& error)
{
	OnResponseMacro(MSG_ID_QUERY_ORDER, nRequestId, accountID, &nRequestId, data, &isLast, &error)
}

///请求查询成交响应
void XTTradeProcessor::onReqDealDetail(const char* accountID, int nRequestId, const CDealDetail* data, bool isLast, const XtError& error)
{
	OnResponseMacro(MSG_ID_QUERY_TRADE, nRequestId, accountID, &nRequestId, data, &isLast, &error)
}

///请求查询投资者持仓响应
void XTTradeProcessor::onReqPositionDetail(const char* accountID, int nRequestId, const CPositionDetail* data, bool isLast, const XtError& error)
{
	if (data)
	{
		auto msgId = nRequestId == -1 ? MSG_ID_POSITION_UPDATED : MSG_ID_QUERY_POSITION;

		if (isLast)
			DataLoadMask |= DataLoadType::POSITION_DATA_LOADED;

		OnResponseMacro(msgId, nRequestId, accountID, &nRequestId, data, &isLast, &error);
	}
}

///请求查询资金账户响应
void XTTradeProcessor::onReqAccountDetail(const char* accountID, int nRequestId, const CAccountDetail* data, bool isLast, const XtError& error)
{
	OnResponseMacro(MSG_ID_QUERY_ACCOUNT_INFO, nRequestId, accountID, data, &isLast, &error)
}

void XTTradeProcessor::onRtnLoginStatus(const char * accountID, EBrokerLoginStatus status, int brokerType, const char * errorMsg)
{
	getMessageSession()->getUserInfo().setInvestorId(accountID);
}

///报单通知
void XTTradeProcessor::onRtnOrderDetail(const COrderDetail* data)
{
	OnResponseMacro(MSG_ID_ORDER_UPDATE, 0, data);
}

///成交通知
void XTTradeProcessor::onRtnDealDetail(const CDealDetail* data)
{
	_tradeCnt++;

	OnResponseMacro(MSG_ID_TRADE_RTN, 0, data);

	QueryUserPositionAsyncIfNeed();
}


///报单录入错误回报
void XTTradeProcessor::onRtnOrderError(const COrderError* data)
{
	OnResponseMacro(MSG_ID_ORDER_NEW, 0, data)
}

///报单操作错误回报
void XTTradeProcessor::onRtnCancelError(const CCancelError* data)
{
	OnResponseMacro(MSG_ID_ORDER_CANCEL, 0, data)
}