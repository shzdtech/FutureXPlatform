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

#include "../databaseop/TradeDAO.h"

#include "../litelogger/LiteLogger.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Purpose:    Implementation of CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::CTPTradeWorkerProcessor(IServerContext* pServerCtx)
	: _userSessionCtn_Ptr(SessionContainer<std::string>::NewInstancePtr())
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
		std::this_thread::sleep_for(std::chrono::seconds(1)); //wait 1 secs for connecting to CTP server
	}

	LoadDataAsync();
}

int CTPTradeWorkerProcessor::RequestExchangeData(void)
{
	int ret = -1;

	if (HasLogged())
	{
		// LOG_INFO << _serverCtx->getServerUri() << ": Try loading data...";
		auto trdAPI = _rawAPI->TrdAPI;

		auto delay = std::chrono::seconds(3);

		/*std::this_thread::sleep_for(delay);
		CThostFtdcQryTradingAccountField reqaccount{};
		ret = trdAPI->ReqQryTradingAccount(&reqaccount, 0);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryInvestorPositionField reqposition{};
		ret = trdAPI->ReqQryInvestorPosition(&reqposition, 0);*/

		std::this_thread::sleep_for(delay);
		CThostFtdcQryExchangeField reqexchange{};
		ret = trdAPI->ReqQryExchange(&reqexchange, 0);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryOrderField reqorder{};
		ret = trdAPI->ReqQryOrder(&reqorder, 0);

		std::this_thread::sleep_for(delay);
		CThostFtdcQryTradeField reqtrd{};
		ret = trdAPI->ReqQryTrade(&reqtrd, 0);

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

int CTPTradeWorkerProcessor::LoadDataAsync(void)
{
	if (!DataLoaded)
	{
		auto trdAPI = _rawAPI->TrdAPI;
		_initializer = std::move(std::thread([trdAPI, this]() {
			auto millsec = std::chrono::milliseconds(500);
			while (!DataLoaded)
			{
				if (!HasLogged())
				{
					if (!CTPUtility::HasReturnError(LoginSystemUser()))
					{
						std::this_thread::sleep_for(std::chrono::seconds(3)); //wait 3 secs for login CTP server
						if (!DataLoaded)
						{
							RequestExchangeData();
						}
					}
				}

				for (int cum_ms = 0; cum_ms < RetryInterval && !DataLoaded; cum_ms += millsec.count())
				{
					std::this_thread::sleep_for(millsec);
				}
			}
		}));
	}

	return 0;
}

void CTPTradeWorkerProcessor::OnDataLoaded(void)
{
	LOG_INFO << getServerContext()->getServerUri() << ": Data preloading finished.";

	std::string logout_dataloaded;
	if (getServerContext()->getConfigVal("logout_dataloaded", logout_dataloaded) &&
		stringutility::compare(logout_dataloaded.data(), "true") == 0)
	{
		CThostFtdcUserLogoutField logout{};
		std::strcpy(logout.BrokerID, _systemUser.getBrokerId().data());
		std::strcpy(logout.UserID, _systemUser.getUserId().data());
		_rawAPI->TrdAPI->ReqUserLogout(&logout, 0);
		_isLogged = false;

		LOG_INFO << getServerContext()->getServerUri() << ": System user has logout.";
	}

	LOG_FLUSH;
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
		LOG_FLUSH;
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
				OnDataLoaded();
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
		if (auto orderptr = CTPUtility::ParseRawOrderInput(pInputOrder, pRspInfo, _systemUser.getSessionId(),
			_userOrderCtx.FindOrder(CTPUtility::ToUInt64(pInputOrder->OrderRef))))
		{
			orderptr->HasMore = false;
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
		}
	}
}

///报单操作错误回报
void CTPTradeWorkerProcessor::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	if (pOrderAction)
	{
		if (pOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			if (auto orderptr = CTPUtility::ParseRawOrderAction(pOrderAction, pRspInfo,
				_userOrderCtx.FindOrder(CTPUtility::ToUInt64(pOrderAction->OrderRef))))
			{
				orderptr->HasMore = false;
				DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
			}
		}
	}
}

///报单录入请求响应
void CTPTradeWorkerProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		if (auto orderptr = CTPUtility::ParseRawOrderInput(pInputOrder, pRspInfo,
			_systemUser.getSessionId(),
			_userOrderCtx.FindOrder(CTPUtility::ToUInt64(pInputOrder->OrderRef))))
		{
			orderptr->HasMore = !bIsLast;
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
		}
	}
}

void CTPTradeWorkerProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		if (pInputOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			if (auto orderptr = CTPUtility::ParseRawOrderInputAction(pInputOrderAction, pRspInfo,
				_userOrderCtx.FindOrder(CTPUtility::ToUInt64(pInputOrderAction->OrderRef))))
			{
				orderptr->HasMore = !bIsLast;
				DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
			}
		}
	}
}

void CTPTradeWorkerProcessor::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder)
	{
		if (auto orderptr = CTPUtility::ParseRawOrder(pOrder,
			_userOrderCtx.FindOrder(CTPUtility::ToUInt64(pOrder->OrderRef))))
		{
			orderptr->HasMore = false;
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
		}
	}
}

void CTPTradeWorkerProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		if (auto trdDO_Ptr = CTPUtility::ParseRawTrade(pTrade))
		{
			if (auto order_ptr = _userOrderCtx.FindOrder(trdDO_Ptr->OrderID))
			{
				trdDO_Ptr->SetUserID(order_ptr->UserID());
				trdDO_Ptr->SetPortfolioID(order_ptr->PortfolioID());
			}
			TradeDAO::SaveExchangeTrade(*trdDO_Ptr);
			_userTradeCtx.AddTrade(*trdDO_Ptr);
			DispatchUserMessage(MSG_ID_TRADE_RTN, 0, trdDO_Ptr->UserID(), trdDO_Ptr);
		}
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

UserTradeContext& CTPTradeWorkerProcessor::GetUserTradeContext()
{
	return _userTradeCtx;
}

UserOrderContext & CTPTradeWorkerProcessor::GetUserOrderContext(void)
{
	return _userOrderCtx;
}

void CTPTradeWorkerProcessor::DispatchUserMessage(int msgId, int serialId, const std::string& userId,
	const dataobj_ptr& dataobj_ptr)
{
	_userSessionCtn_Ptr->foreach(userId, [msgId, serialId, dataobj_ptr, this](IMessageSession* pSession)
	{this->SendDataObject(pSession, msgId, serialId, dataobj_ptr); });
}


