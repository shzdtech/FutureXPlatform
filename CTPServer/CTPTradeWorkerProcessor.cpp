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

CTPTradeWorkerProcessor::CTPTradeWorkerProcessor(const std::map<std::string, std::string>& configMap,
	IServerContext* pServerCtx)
	: CTPTradeProcessor(configMap),
	_userSessionCtn_Ptr(SessionContainer<std::string>::NewInstance())
{
	_serverCtx = pServerCtx,
	DataLoaded = false;
	_systemUser.setBrokerId(SysParam::Get(CTP_TRADER_BROKERID));
	_systemUser.setInvestorId(SysParam::Get(CTP_TRADER_USERID));
	_systemUser.setUserId(SysParam::Get(CTP_TRADER_USERID));
	_systemUser.setPassword(SysParam::Get(CTP_TRADER_PASSWORD));
	_systemUser.setServer(SysParam::Get(CTP_TRADER_SERVER));
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Purpose:    Implementation of CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::OnInit()
// Purpose:    Implementation of CTPTradeWorkerProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPTradeWorkerProcessor::Initialize(void)
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
	_initializer = std::async(std::launch::async, [trdAPI, this]() {
		std::this_thread::sleep_for(std::chrono::seconds(1)); //wait 1 secs for connecting to CTP server
		while (!this->DataLoaded)
		{
			if (!this->HasLogged())
			{
				LoginSystemUser();
				std::this_thread::sleep_for(std::chrono::seconds(3)); //wait 5 secs for login CTP server
			}

			if (!DataLoaded)
			{
				LoadSystemData(trdAPI);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(this->RetryInterval));
		}
	});
}

int CTPTradeWorkerProcessor::LoadSystemData(CThostFtdcTraderApi * trdAPI)
{
	auto delay = std::chrono::seconds(2);

	std::this_thread::sleep_for(delay);
	CThostFtdcQryTradingAccountField reqaccount{};
	trdAPI->ReqQryTradingAccount(&reqaccount, 0);

	std::this_thread::sleep_for(delay);
	CThostFtdcQryInvestorPositionField reqposition{};
	trdAPI->ReqQryInvestorPosition(&reqposition, 0);

	std::this_thread::sleep_for(delay);
	CThostFtdcQryTradeField reqtrd{};
	trdAPI->ReqQryTrade(&reqtrd, 0);

	std::this_thread::sleep_for(delay);
	CThostFtdcQryOrderField reqorder{};
	trdAPI->ReqQryOrder(&reqorder, 0);

	std::this_thread::sleep_for(delay);
	CThostFtdcQryExchangeField reqexchange{};
	trdAPI->ReqQryExchange(&reqexchange, 0);

	std::this_thread::sleep_for(delay);
	CThostFtdcQryInstrumentField reqinstr{};
	trdAPI->ReqQryInstrument(&reqinstr, 0);

	return 0;
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
			session->setLoginStatus(true);
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

		_isLogged = true;
		auto trdAPI = _rawAPI->TrdAPI;

		CThostFtdcSettlementInfoConfirmField reqsettle{};
		std::strcpy(reqsettle.BrokerID, _systemUser.getBrokerId().data());
		std::strcpy(reqsettle.InvestorID, _systemUser.getInvestorId().data());
		trdAPI->ReqSettlementInfoConfirm(&reqsettle, 0);
	}
}

void CTPTradeWorkerProcessor::OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	try
	{
		DataLoaded = bIsLast;
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
	if (pMessageSession->IsLogin() && _isLogged)
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


