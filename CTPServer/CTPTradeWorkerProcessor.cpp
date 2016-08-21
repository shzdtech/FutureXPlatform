/***********************************************************************
 * Module:  CTPTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Implementation of the class CTPTradeWorkerProcessor
 ***********************************************************************/

#include "CTPTradeWorkerProcessor.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
#include "../dataobject/TradeRecordDO.h"
#include "../common/Attribute_Key.h"
#include "../message/AppContext.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"

#include "../litelogger/LiteLogger.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Purpose:    Implementation of CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::CTPTradeWorkerProcessor(IServerContext* pServerCtx)
	: _userSessionCtn_Ptr(SessionContainer<std::string>::NewInstance())
{
	_serverCtx = pServerCtx;
	DataLoaded = false;

	std::string value;
	if (!_serverCtx->getConfigVal(CTP_TRADER_BROKERID, value))
		value = SysParam::Get(CTP_TRADER_BROKERID);
	_systemUser.setBrokerId(value);

	if (!_serverCtx->getConfigVal(CTP_TRADER_USERID, value))
		value = SysParam::Get(CTP_TRADER_USERID);
	_systemUser.setInvestorId(value);
	_systemUser.setUserId(value);

	if (!_serverCtx->getConfigVal(CTP_TRADER_PASSWORD, value))
		value = SysParam::Get(CTP_TRADER_PASSWORD);
	_systemUser.setPassword(value);

	if (!_serverCtx->getConfigVal(CTP_TRADER_SERVER, value))
		value = SysParam::Get(CTP_TRADER_SERVER);
	_systemUser.setServer(value);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Purpose:    Implementation of CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
{
	DataLoaded = true;
	_initializer.join();
	LOG_DEBUG << __FUNCTION__;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::OnInit()
// Purpose:    Implementation of CTPTradeWorkerProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPTradeWorkerProcessor::Initialize(IServerContext* pServerCtx)
{
	if (!_rawAPI->TrdAPI) {
		_rawAPI->TrdAPI = CThostFtdcTraderApi::CreateFtdcTraderApi();
		_rawAPI->TrdAPI->RegisterSpi(this);
		_rawAPI->TrdAPI->RegisterFront(const_cast<char*> (_systemUser.getServer().data()));
		_rawAPI->TrdAPI->SubscribePrivateTopic(THOST_TERT_RESUME);
		_rawAPI->TrdAPI->SubscribePublicTopic(THOST_TERT_RESUME);
		_rawAPI->TrdAPI->Init();
	}

	if (_systemUser.getPassword().length() < 1)
		return;

	auto trdAPI = _rawAPI->TrdAPI;
	_initializer = std::move(std::thread([trdAPI, this]() {
		std::this_thread::sleep_for(std::chrono::seconds(1)); //wait 1 secs for connecting to CTP server
		auto millsec = std::chrono::milliseconds(500);
		while (!this->DataLoaded)
		{
			if (!this->HasLogged())
			{
				LoginSystemUser();
				std::this_thread::sleep_for(std::chrono::seconds(5)); //wait 5 secs for login CTP server
			}

			if (!DataLoaded)
			{
				LoadSystemData(trdAPI);
			}

			for (int cum_ms = 0; cum_ms < this->RetryInterval && !DataLoaded; cum_ms += millsec.count())
			{
				std::this_thread::sleep_for(millsec);
			}
		}
	}));
}

int CTPTradeWorkerProcessor::LoadSystemData(CThostFtdcTraderApi * trdAPI)
{
	int ret = -1;

	if (HasLogged())
	{
		// LOG_INFO << _serverCtx->getServerUri() << ": Try loading data...";

		auto delay = std::chrono::seconds(2);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryTradingAccountField reqaccount{};
		ret = trdAPI->ReqQryTradingAccount(&reqaccount, 0);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryInvestorPositionField reqposition{};
		ret = trdAPI->ReqQryInvestorPosition(&reqposition, 0);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryTradeField reqtrd{};
		ret = trdAPI->ReqQryTrade(&reqtrd, 0);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryOrderField reqorder{};
		ret = trdAPI->ReqQryOrder(&reqorder, 0);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryExchangeField reqexchange{};
		ret = trdAPI->ReqQryExchange(&reqexchange, 0);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryInstrumentField reqinstr{};
		ret = trdAPI->ReqQryInstrument(&reqinstr, 0);
	}

	return ret;
}

int CTPTradeWorkerProcessor::LoginSystemUser(void)
{
	CThostFtdcReqUserLoginField req{};
	std::strcpy(req.BrokerID, _systemUser.getBrokerId().data());
	std::strcpy(req.UserID, _systemUser.getUserId().data());
	std::strcpy(req.Password, _systemUser.getPassword().data());
	return _rawAPI->TrdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
}

int CTPTradeWorkerProcessor::LoginSystemUserIfNeed(void)
{
	int ret = 0;

	if (!_isLogged)
		ret = LoginSystemUser();

	return ret;
}

///登录请求响应
void CTPTradeWorkerProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!CTPUtility::HasError(pRspInfo))
	{
		if (auto session = LockMessageSession())
		{
			session->setLoginTimeStamp();
			auto userinfo_ptr = session->getUserInfo();
			userinfo_ptr->setBrokerId(pRspUserLogin->BrokerID);
			userinfo_ptr->setInvestorId(pRspUserLogin->UserID);
			userinfo_ptr->setUserId(pRspUserLogin->UserID);
			userinfo_ptr->setPermission(ALLOW_TRADING);
			userinfo_ptr->setFrontId(pRspUserLogin->FrontID);
			userinfo_ptr->setSessionId(pRspUserLogin->SessionID);

			_systemUser.setFrontId(pRspUserLogin->FrontID);
			_systemUser.setSessionId(pRspUserLogin->SessionID);
		}

		CThostFtdcSettlementInfoConfirmField reqsettle{};
		std::strcpy(reqsettle.BrokerID, _systemUser.getBrokerId().data());
		std::strcpy(reqsettle.InvestorID, _systemUser.getInvestorId().data());
		_rawAPI->TrdAPI->ReqSettlementInfoConfirm(&reqsettle, 0);

		_isLogged = true;

		LOG_INFO << getServerContext()->getServerUri() << ": System user has logged.";
	}
}

void CTPTradeWorkerProcessor::OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	try
	{
		if (!DataLoaded)
		{
			if (DataLoaded = bIsLast)
			{
				LOG_INFO << getServerContext()->getServerUri() << ": Data preloading finished.";
				LOG_FLUSH;
			}
		}

		ProcessResponseMacro(this, MSG_ID_QUERY_INSTRUMENT, nRequestID, pInstrument, pRspInfo, &nRequestID, &bIsLast);
	}
	catch (...) {}
}

///报单录入错误回报
void CTPTradeWorkerProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if (pInputOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrderInput(pInputOrder, pRspInfo, _systemUser.getSessionId());
		DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
	}
}

///报单操作错误回报
void CTPTradeWorkerProcessor::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	if (pOrderAction)
	{
		if (pOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			auto orderptr = CTPUtility::ParseRawOrderAction(pOrderAction, pRspInfo);
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
		}
	}
}

///报单录入请求响应
void CTPTradeWorkerProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrderInput(pInputOrder, pRspInfo, _systemUser.getSessionId());

		orderptr->HasMore = !bIsLast;
		DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
	}
}

void CTPTradeWorkerProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		if (pInputOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			auto orderptr = CTPUtility::ParseRawOrderInputAction(pInputOrderAction, pRspInfo);

			orderptr->HasMore = !bIsLast;
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
		}
	}
}

void CTPTradeWorkerProcessor::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrder(pOrder);
		auto& orderDO = _userOrderMap.getorfill(orderptr->UserID()).
			getorfill(orderptr->OrderSysID, *orderptr);
		orderDO = *orderptr;
		DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
	}
}

void CTPTradeWorkerProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		auto trdDO_Ptr = CTPUtility::ParseRawTrade(pTrade);
		_userTradeMap.getorfill(trdDO_Ptr->UserID()).
			getorfill(trdDO_Ptr->TradeID, *trdDO_Ptr);
		DispatchUserMessage(MSG_ID_TRADE_RTN, 0, pTrade->UserID, trdDO_Ptr);
	}
}

void CTPTradeWorkerProcessor::RegisterLoggedSession(IMessageSession * pMessageSession)
{
	if (pMessageSession->getLoginTimeStamp() && _isLogged)
	{
		if (auto userInfoPtr = pMessageSession->getUserInfo())
		{
			userInfoPtr->setBrokerId(_systemUser.getBrokerId());
			userInfoPtr->setInvestorId(_systemUser.getInvestorId());
			userInfoPtr->setFrontId(_systemUser.getFrontId());
			userInfoPtr->setSessionId(_systemUser.getSessionId());
			_userSessionCtn_Ptr->add(userInfoPtr->getUserId(), pMessageSession);
		}
	}
}

std::vector<AccountInfoDO>& CTPTradeWorkerProcessor::GetAccountInfo(const std::string userId)
{
	return _accountInfoMap.getorfill(userId);
}

std::set<ExchangeDO>& CTPTradeWorkerProcessor::GetExchangeInfo()
{
	return _exchangeInfo_Set;
}

UserPositionExDOMap& CTPTradeWorkerProcessor::GetUserPositionMap()
{
	return _userPositionMap;
}

autofillmap<uint64_t, TradeRecordDO>& CTPTradeWorkerProcessor::GetUserTradeMap(const std::string userId)
{
	return _userTradeMap.getorfill(userId);
}

autofillmap<uint64_t, OrderDO>& CTPTradeWorkerProcessor::GetUserOrderMap(const std::string userId)
{
	return _userOrderMap.getorfill(userId);
}

void CTPTradeWorkerProcessor::DispatchUserMessage(int msgId, int serialId, const std::string& userId,
	const dataobj_ptr& dataobj_ptr)
{
	_userSessionCtn_Ptr->foreach(userId, [msgId, serialId, dataobj_ptr, this](IMessageSession* pSession)
	{this->SendDataObject(pSession, msgId, serialId, dataobj_ptr); });
}


