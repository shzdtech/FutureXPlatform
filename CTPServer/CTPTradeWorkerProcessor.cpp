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
#include "../systemsettings/AppContext.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../message/MessageSession.h"
#include "../bizutility/ExchangeRouterTable.h"
#include "../bizutility/PositionPortfolioMap.h"
#include "../databaseop/SysParamsDAO.h"
#include "../databaseop/VersionDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/TradeDAO.h"
#include "../databaseop/PositionDAO.h"
#include "../ordermanager/OrderSeqGen.h"
#include "../ordermanager/OrderPortfolioCache.h"
#include "../bizutility/ModelParamsCache.h"
#include "../riskmanager/RiskModelAlgorithmManager.h"
#include "../riskmanager/RiskContext.h"
#include "../riskmanager/RiskModelParams.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Purpose:    Implementation of CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::CTPTradeWorkerProcessor(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx)
	: _sharedProcHub(4), _sharedSystemSessionHub(SessionContainer<std::string>::NewInstancePtr()), _isQueryAccount(true),
	CTPTradeWorkerProcessorBase(pServerCtx, positionCtx)
{
	_serverCtx = pServerCtx;

	if (_isQueryAccount)
		_accountQuery = std::async(std::launch::async, &CTPTradeWorkerProcessor::QueryAccountWorker, this);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Purpose:    Implementation of CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
{
	_closing = true;

	if (_accountQuery.valid())
		_accountQuery.wait();

	LOG_DEBUG << __FUNCTION__;
}

void CTPTradeWorkerProcessor::Initialize(IServerContext * pServerCtx)
{
	Init(pServerCtx);
}

void CTPTradeWorkerProcessor::QueryAccountWorker()
{
	while (!_closing)
	{
		for (auto pair : _sharedProcHub.lock_table())
		{
			CThostFtdcQryTradingAccountField req{};
			//std::strncpy(req.BrokerID, session_ptr->getUserInfo().getBrokerId().data(), sizeof(req.BrokerID));
			//std::strncpy(req.InvestorID, session_ptr->getUserInfo().getInvestorId.data(), sizeof(req.InvestorID));
			if (auto proc = pair.second)
			{
				if (proc->Logged())
				{
					if (auto tdApi = ((CTPRawAPI*)proc->getRawAPI())->TdAPIProxy())
						tdApi->get()->ReqQryTradingAccount(&req, 0);
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

int CTPTradeWorkerProcessor::RequestData(void)
{
	int ret = -1;

	if (HasLogged())
	{
		LOG_INFO << _serverCtx->getServerUri() << ": Try preloading data...";
		auto trdAPI = TradeApi();

		auto session = getMessageSession();
		while (!session)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			session = getMessageSession();
		}

		if (!session->getLoginTimeStamp())
		{
			session->setLoginTimeStamp();
			auto& userInfo = session->getUserInfo();
			userInfo.setUserId(_systemUser.getUserId());
			userInfo.setBrokerId(_systemUser.getBrokerId());
			userInfo.setInvestorId(_systemUser.getInvestorId());
			userInfo.setPermission(ALLOW_TRADING);
			userInfo.setFrontId(_systemUser.getFrontId());
			userInfo.setSessionId(_systemUser.getSessionId());
			userInfo.setTradingDay(_systemUser.getTradingDay());
		}

		if (!(GetDataLoadMask() & INSTRUMENT_DATA_LOADED))
		{
			CThostFtdcQryInstrumentField reqinstr{};
			ret = trdAPI->get()->ReqQryInstrument(&reqinstr, 0);
		}

		//std::this_thread::sleep_for(delay);
		//CThostFtdcQryTradingAccountField reqaccount{};
		//ret = trdAPI->ReqQryTradingAccount(&reqaccount, 0);

		/*if (!(GetDataLoadMask() & POSITION_DATA_LOADED))
		{
			std::this_thread::sleep_for(DefaultQueryTime);
			CThostFtdcQryInvestorPositionField reqposition{};
			ret = trdAPI->get()->ReqQryInvestorPosition(&reqposition, 0);
		}*/

		//if (!(GetDataLoadMask() & EXCHANGE_DATA_LOADED))
		//{
		//	std::this_thread::sleep_for(DefaultQueryTime);
		//	CThostFtdcQryExchangeField reqexchange{};
		//	ret = trdAPI->ReqQryExchange(&reqexchange, 0);
		//	if (ret == 0)
		//		GetDataLoadMask() |= EXCHANGE_DATA_LOADED;
		//	else
		//		return ret;
		//}

		//if (!(GetDataLoadMask() & ORDER_DATA_LOADED))
		//{
		//	std::this_thread::sleep_for(DefaultQueryTime);
		//	CThostFtdcQryOrderField reqorder{};
		//	ret = trdAPI->ReqQryOrder(&reqorder, 0);
		//	if (ret == 0)
		//		GetDataLoadMask() |= ORDER_DATA_LOADED;
		//	else
		//		return ret;
		//}

		//if (!(GetDataLoadMask() & TRADE_DATA_LOADED))
		//{
		//	std::this_thread::sleep_for(DefaultQueryTime);
		//	CThostFtdcQryTradeField reqtrd{};
		//	ret = trdAPI->ReqQryTrade(&reqtrd, 0);
		//	if (ret == 0)
		//		GetDataLoadMask() |= TRADE_DATA_LOADED;
		//	else
		//		return ret;
		//}
	}

	return ret;
}

int CTPTradeWorkerProcessor::LoginSystemUser(void)
{
	int ret = 0;

	if (!_authCode.empty())
	{
		CThostFtdcReqAuthenticateField reqAuth{};
		std::strncpy(reqAuth.BrokerID, _systemUser.getBrokerId().data(), sizeof(reqAuth.BrokerID));
		std::strncpy(reqAuth.UserID, _systemUser.getUserId().data(), sizeof(reqAuth.UserID));
		std::strncpy(reqAuth.AuthCode, _authCode.data(), sizeof(reqAuth.AuthCode));
		std::strncpy(reqAuth.UserProductInfo, _productInfo.data(), sizeof(reqAuth.UserProductInfo));
		ret = TradeApi()->get()->ReqAuthenticate(&reqAuth, 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (ret == 0)
	{
		CThostFtdcReqUserLoginField req{};
		std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
		std::strncpy(req.UserID, _systemUser.getInvestorId().data(), sizeof(req.UserID));
		std::strncpy(req.Password, _systemUser.getPassword().data(), sizeof(req.Password));
		std::strncpy(req.UserProductInfo, _productInfo.data(), sizeof(req.UserProductInfo));

		ret = TradeApi()->get()->ReqUserLogin(&req, 0);
	}

	return ret;
}

int CTPTradeWorkerProcessor::LoginSystemUserIfNeed(void)
{
	std::lock_guard<std::mutex> lock(_loginMutex);

	int ret = 0;

	if (!_isLogged || !_isConnected)
	{
		std::string address(_systemUser.getServer());
		CreateBackendAPI(this, _systemUser.getInvestorId(), address);
		ret = LoginSystemUser();
		if (ret == -1)
		{
			LOG_WARN << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId()
				<< " cannot connect to trading server at: " << address;

			std::string defaultCfg;
			_serverCtx->getConfigVal(ExchangeRouterTable::TARGET_TD_AM, defaultCfg);
			ExchangeRouterDO exDO;
			if (ExchangeRouterTable::TryFind(defaultCfg, exDO))
			{
				CreateBackendAPI(this, _systemUser.getInvestorId(), exDO.Address);
				ret = LoginSystemUser();
			}

			if (ret == 0)
			{
				LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId()
					<< " has connected to post trading server at: " << address;
			}
			else
			{
				LOG_WARN << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId()
					<< " cannot connect to post trading server at: " << address;
			}
		}
		else
		{
			LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId()
				<< " has connected to trading server at: " << address;
		}
	}

	return ret;
}

int CTPTradeWorkerProcessor::LogoutSystemUser(void)
{
	CThostFtdcUserLogoutField logout{};
	std::strncpy(logout.BrokerID, _systemUser.getBrokerId().data(), sizeof(logout.BrokerID));
	std::strncpy(logout.UserID, _systemUser.getInvestorId().data(), sizeof(logout.UserID));
	return TradeApi()->get()->ReqUserLogout(&logout, 0);
}

int & CTPTradeWorkerProcessor::GetDataLoadMask(void)
{
	return DataLoadMask;
}

TemplateMessageProcessor * CTPTradeWorkerProcessor::GetTemplateProcessor(void)
{
	return this;
}

IServerContext * CTPTradeWorkerProcessor::getServerContext(void)
{
	return CTPTradeProcessor::getServerContext();
}


OrderDO_Ptr CTPTradeWorkerProcessor::RefineOrder(CThostFtdcOrderField *pOrder)
{
	// update orders
	auto orderSysId = CTPUtility::ToUInt64(pOrder->OrderSysID);
	OrderDO_Ptr ctxOrder = orderSysId ? _userOrderCtx.FindOrder(orderSysId) : nullptr;
	auto orderptr = CTPUtility::ParseRawOrder(pOrder, ctxOrder);

	if (orderptr->PortfolioID().empty())
	{
		PortfolioKey portfolio;
		if (OrderPortfolioCache::Find(OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID), portfolio))
		{
			orderptr->SetPortfolioID(portfolio.PortfolioID());
		}
		else if (orderSysId && OrderPortfolioCache::Find(orderSysId, portfolio))
		{
			orderptr->SetPortfolioID(portfolio.PortfolioID());
		}
	}

	//if (orderptr->PortfolioID().empty())
	//{
	//	if (auto pPortfolioKey = PositionPortfolioMap::FindPortfolio(orderptr->UserID(), orderptr->InstrumentID()))
	//	{
	//		orderptr->SetPortfolioID(pPortfolioKey->PortfolioID());
	//	}
	//}

	if (orderSysId)
	{
		_userOrderCtx.UpsertOrder(orderSysId, orderptr);
	}

	return orderptr;
}

TradeRecordDO_Ptr CTPTradeWorkerProcessor::RefineTrade(CThostFtdcTradeField * pTrade)
{
	TradeRecordDO_Ptr trdDO_Ptr = CTPUtility::ParseRawTrade(pTrade);

	if (trdDO_Ptr)
	{
		PortfolioKey portfolio;
		if (OrderPortfolioCache::Find(trdDO_Ptr->OrderSysID, portfolio))
		{
			trdDO_Ptr->SetPortfolioID(portfolio.PortfolioID());
		}
		else if (auto order_ptr = _userOrderCtx.FindOrder(trdDO_Ptr->OrderSysID))
		{
			if (!order_ptr->PortfolioID().empty())
			{
				trdDO_Ptr->SetPortfolioID(order_ptr->PortfolioID());
			}
			else if (OrderPortfolioCache::Find(OrderSeqGen::GetOrderID(order_ptr->OrderID, order_ptr->SessionID), portfolio))
			{
				trdDO_Ptr->SetPortfolioID(portfolio.PortfolioID());
			}
		}
		else if (OrderPortfolioCache::Find(OrderSeqGen::GetOrderID(trdDO_Ptr->OrderID, _systemUser.getSessionId()), portfolio))
		{
			trdDO_Ptr->SetPortfolioID(portfolio.PortfolioID());
		}

		//if (trdDO_Ptr->PortfolioID().empty())
		//{
		//	if (auto pPortfolioKey = PositionPortfolioMap::FindPortfolio(trdDO_Ptr->UserID(), trdDO_Ptr->InstrumentID()))
		//	{
		//		trdDO_Ptr->SetPortfolioID(pPortfolioKey->PortfolioID());
		//	}
		//}
	}

	return trdDO_Ptr;
}

CTPTradeWorkerSAProcessor_Ptr CTPTradeWorkerProcessor::CreateSAProcessor()
{
	return std::make_shared<CTPTradeWorkerSAProcessor>(this);
}

void CTPTradeWorkerProcessor::LoginUserSession(const CTPProcessor_Ptr& proc_ptr,
	const std::string & brokerId, const std::string & investorId, const std::string & password, const std::string & serverName)
{
	auto& userInfo = proc_ptr->getMessageSession()->getUserInfo();

	CTPTradeWorkerSAProcessor_Ptr tradeProc_Ptr;

	if (!_sharedProcHub.find(investorId, tradeProc_Ptr))
	{
		tradeProc_Ptr = CreateSAProcessor();
		tradeProc_Ptr->setServerContext(proc_ptr->getServerContext());
		tradeProc_Ptr->setServiceLocator(proc_ptr->getServiceLocator());
		auto msgSession = std::make_shared<MessageSession>(getMessageSession()->getSessionManager());
		msgSession->RegisterProcessor(tradeProc_Ptr);

		_sharedProcHub.insert(investorId, tradeProc_Ptr);
		_sharedProcHub.find(investorId, tradeProc_Ptr);
	}

	if (!tradeProc_Ptr->Logged())
	{
		int ret = tradeProc_Ptr->Login(brokerId, investorId, password, serverName);
		CTPUtility::CheckReturnError(ret);
		UpdateSharedUserInfo(investorId, tradeProc_Ptr);
	}

	auto& sysUser = tradeProc_Ptr->getSystemUser();
	userInfo.setFrontId(sysUser.getFrontId());
	userInfo.setSessionId(sysUser.getSessionId());
	userInfo.setTradingDay(sysUser.getTradingDay());
	proc_ptr->setRawAPI_Ptr(tradeProc_Ptr->RawAPI_Ptr());
	_sharedSystemSessionHub->add(investorId, proc_ptr->getMessageSession());
}

void CTPTradeWorkerProcessor::UpdateSharedUserInfo(const std::string& investorId, const CTPTradeWorkerSAProcessor_Ptr& proc_ptr)
{
	auto api_ptr = proc_ptr->RawAPI_Ptr();
	auto& sysUser = proc_ptr->getSystemUser();
	_sharedSystemSessionHub->forall([&api_ptr, &sysUser, this](const IMessageSession_Ptr& session_ptr)
	{
		auto proc_ptr = std::static_pointer_cast<CTPProcessor>(session_ptr->LockMessageProcessor());
		proc_ptr->setRawAPI_Ptr(api_ptr);
		auto& userInfo = session_ptr->getUserInfo();
		userInfo.setFrontId(sysUser.getFrontId());
		userInfo.setSessionId(sysUser.getSessionId());
		userInfo.setTradingDay(sysUser.getTradingDay());
	}
	);
}

bool CTPTradeWorkerProcessor::IsUserLogged(const std::string& userId)
{
	return _userSessionCtn_Ptr->contains(userId);
}






///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CTPTradeWorkerProcessor::OnFrontConnected()
{
	_isConnected = true;

	auto loginTime = _systemUser.getLoginTime();
	auto now = std::time(nullptr);
	if (_isLogged && (now - loginTime) > 600)
	{
		LoginSystemUser();
	}

	LOG_INFO << getServerContext()->getServerUri() << ": Front Connected.";
}

void CTPTradeWorkerProcessor::OnFrontDisconnected(int nReason)
{
	_isConnected = false;
	if (nReason)
	{
		std::string errMsg;
		CTPUtility::LogFrontDisconnected(nReason, errMsg);
		LOG_WARN << getServerContext()->getServerUri() << " has disconnected: " << errMsg;
	}
}

void CTPTradeWorkerProcessor::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (CTPUtility::HasError(pRspInfo))
	{
		LOG_ERROR << getServerContext()->getServerUri() << ": " << pRspInfo->ErrorMsg;
	}
	else
	{
		LOG_INFO << getServerContext()->getServerUri() << " has successfully authenticated.";
	}

	//CThostFtdcReqUserLoginField req{};
	//std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
	//std::strncpy(req.UserID, _systemUser.getInvestorId().data(), sizeof(req.UserID));
	//std::strncpy(req.Password, _systemUser.getPassword().data(), sizeof(req.Password));
	//std::strncpy(req.UserProductInfo, _productInfo.data(), sizeof(req.UserProductInfo));

	//std::static_pointer_cast<CTPRawAPI>(_rawAPI)->get()->ReqUserLogin(&req, 0);
}

///登录请求响应
void CTPTradeWorkerProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (CTPUtility::HasError(pRspInfo))
	{
		LOG_ERROR << _serverCtx->getServerUri() << ": " << pRspInfo->ErrorMsg;
	}
	else
	{
		std::string defaultUserId;
		if (SysParam::TryGet("DEFAULT.TRADINGDESK.ACCOUNTID", defaultUserId))
		{
			_systemUser.setUserId(defaultUserId);
		}

		_systemUser.setFrontId(pRspUserLogin->FrontID);
		_systemUser.setSessionId(pRspUserLogin->SessionID);
		_systemUser.setTradingDay(std::atoi(pRspUserLogin->TradingDay));

		if (auto session = getMessageSession())
		{
			session->setLoginTimeStamp();
			auto& userInfo = session->getUserInfo();
			userInfo.setUserId(_systemUser.getUserId());
			userInfo.setBrokerId(_systemUser.getBrokerId());
			userInfo.setInvestorId(_systemUser.getInvestorId());
			userInfo.setPermission(ALLOW_TRADING);
			userInfo.setFrontId(_systemUser.getFrontId());
			userInfo.setSessionId(_systemUser.getSessionId());
			userInfo.setTradingDay(_systemUser.getTradingDay());
		}

		std::string tradingDay;
		SysParam::TryGet(STR_KEY_APP_TRADINGDAY, tradingDay);
		if (tradingDay != pRspUserLogin->TradingDay)
		{
			tradingDay = pRspUserLogin->TradingDay;
			SysParam::Update(STR_KEY_APP_TRADINGDAY, tradingDay);
			SysParamsDAO::UpsertSysParamValue(STR_KEY_APP_TRADINGDAY, tradingDay);
		}

		/*if (_loadPositionFromDB)
			LoadPositonFromDatabase(_systemUser.getUserId(), _systemUser.getInvestorId(),
				std::to_string(_systemUser.getTradingDay()));*/

		CThostFtdcSettlementInfoConfirmField reqsettle{};
		std::strncpy(reqsettle.BrokerID, _systemUser.getBrokerId().data(), sizeof(reqsettle.BrokerID));
		std::strncpy(reqsettle.InvestorID, _systemUser.getInvestorId().data(), sizeof(reqsettle.InvestorID));
		TradeApi()->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

		_isLogged = true;

		LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId() << " has logged on trading server.";
		LOG_FLUSH;
	}
}

void CTPTradeWorkerProcessor::OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	try
	{
		ProcessResponseMacro(this, MSG_ID_QUERY_INSTRUMENT, nRequestID, pInstrument, pRspInfo, &nRequestID, &bIsLast);

		if (bIsLast)
		{
			GetDataLoadMask() |= INSTRUMENT_DATA_LOADED;

			LOG_INFO << _serverCtx->getServerUri() << ": Contract info has been loaded";

			std::string save_contract;

			getServerContext()->getConfigVal("save_contract", save_contract);

			if (!save_contract.empty())
			{
				std::string item = "contract:" + _serverCtx->getServerUri();
				ContractCache::PersistCache(PRODUCT_CACHE_EXCHANGE, item, std::to_string(_systemUser.getTradingDay()));
			}
		}
	}
	catch (...) {}
}

///报单录入错误回报
void CTPTradeWorkerProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if (pInputOrder)
	{
		if (auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, _systemUser.getSessionId()))
		{
			PortfolioKey portfolio;
			auto orderId = OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID);
			if (OrderPortfolioCache::Find(orderId, portfolio))
			{
				orderptr->SetPortfolioID(portfolio.PortfolioID());
				OrderPortfolioCache::Remove(orderId);
			}
			orderptr->HasMore = false;
			DispatchUserMessage(MSG_ID_ORDER_NEW, pInputOrder->RequestID, orderptr->UserID(), orderptr);
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
			if (auto orderptr = CTPUtility::ParseRawOrder(pOrderAction, pRspInfo,
				_userOrderCtx.FindOrder(CTPUtility::ToUInt64(pOrderAction->OrderSysID))))
			{
				orderptr->HasMore = false;
				DispatchUserMessage(MSG_ID_ORDER_CANCEL, pOrderAction->RequestID, orderptr->UserID(), orderptr);
			}
		}
	}
}

///报单录入请求响应
void CTPTradeWorkerProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		if (auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, _systemUser.getSessionId()))
		{
			if (orderptr->ErrorCode)
			{
				PortfolioKey portfolio;
				auto orderId = OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID);
				if (OrderPortfolioCache::Find(orderId, portfolio))
				{
					orderptr->SetPortfolioID(portfolio.PortfolioID());
					OrderPortfolioCache::Remove(orderId);
				}
			}
			orderptr->HasMore = false;
			DispatchUserMessage(MSG_ID_ORDER_NEW, nRequestID, orderptr->UserID(), orderptr);
		}
	}
}

void CTPTradeWorkerProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		if (pInputOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			if (auto orderptr = CTPUtility::ParseRawOrder(pInputOrderAction, pRspInfo,
				_userOrderCtx.FindOrder(CTPUtility::ToUInt64(pInputOrderAction->OrderSysID))))
			{
				orderptr->HasMore = !bIsLast;
				DispatchUserMessage(MSG_ID_ORDER_CANCEL, nRequestID, orderptr->UserID(), orderptr);
			}
		}
	}
}

void CTPTradeWorkerProcessor::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder)
	{
		auto orderptr = RefineOrder(pOrder);

		if (auto msgId = CTPUtility::ParseOrderMessageID(orderptr->OrderStatus))
		{
			auto sid = msgId == MSG_ID_ORDER_CANCEL ? orderptr->OrderSysID : pOrder->RequestID;
			DispatchUserMessage(msgId, sid, orderptr->UserID(), orderptr);
		}

		DispatchUserMessage(MSG_ID_ORDER_UPDATE, pOrder->RequestID, orderptr->UserID(), orderptr);
	}
}

void CTPTradeWorkerProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		_tradeCnt++;

		if (auto trdDO_Ptr = RefineTrade(pTrade))
		{
			PushToLogQueue(trdDO_Ptr);

			DispatchUserMessage(MSG_ID_TRADE_RTN, 0, trdDO_Ptr->UserID(), trdDO_Ptr);

			if (auto position_ptr = UpdatePosition(trdDO_Ptr))
			{
				DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, trdDO_Ptr->UserID(), position_ptr);
			}

			if (!_loadPositionFromDB)
				QueryUserPositionAsyncIfNeed();
		}
	}
}

///请求查询投资者持仓响应
void CTPTradeWorkerProcessor::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::string sysUserId = pInvestorPosition->InvestorID;
	//sysUserId += pInvestorPosition->InvestorID;

	auto position = CTPUtility::ParseRawPosition(pInvestorPosition, sysUserId);
	UpdateSysYdPosition(sysUserId, position);

	if (!IsLoadPositionFromDB())
	{
		std::set<std::string> userSet;
		_sharedSystemSessionHub->foreach(sysUserId, [pInvestorPosition, &position, &sysUserId, &userSet, this](const IMessageSession_Ptr& session_ptr)
		{
			auto& userId = session_ptr->getUserInfo().getUserId();
			if (userSet.find(userId) == userSet.end())
			{
				userSet.emplace(userId);
				position->SetUserID(userId);
				if (position->ExchangeID() == EXCHANGE_SHFE)
				{
					if (pInvestorPosition->PositionDate == THOST_FTDC_PSD_Today)
					{
						position = GetUserPositionContext()->UpsertPosition(userId, *position, false, false);
					}
					else
					{
						position = GetUserPositionContext()->UpsertPosition(userId, *position, true, false);
					}
				}
				else
					position = GetUserPositionContext()->UpsertPosition(userId, *position, false, true);
			}
		});
	}
}

void CTPTradeWorkerProcessor::OnRspQryTradingAccount(CThostFtdcTradingAccountField * pTradingAccount, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	std::shared_ptr<AccountInfoDO> accountInfo_ptr;

	if (pTradingAccount)
	{
		auto pDO = new AccountInfoDO;
		accountInfo_ptr.reset(pDO);

		pDO->BrokerID = pTradingAccount->BrokerID;
		pDO->AccountID = pTradingAccount->AccountID;
		pDO->PreMortgage = pTradingAccount->PreMortgage;
		pDO->PreCredit = pTradingAccount->PreCredit;
		pDO->PreDeposit = pTradingAccount->PreDeposit;
		pDO->PreBalance = pTradingAccount->PreBalance;
		pDO->PreMargin = pTradingAccount->PreMargin;
		pDO->InterestBase = pTradingAccount->InterestBase;
		pDO->Interest = pTradingAccount->Interest;
		pDO->Deposit = pTradingAccount->Deposit;
		pDO->Withdraw = pTradingAccount->Withdraw;
		pDO->FrozenMargin = pTradingAccount->FrozenMargin;
		pDO->FrozenCash = pTradingAccount->FrozenCash;
		pDO->FrozenCommission = pTradingAccount->FrozenCommission;
		pDO->CurrMargin = pTradingAccount->CurrMargin;
		pDO->CashIn = pTradingAccount->CashIn;
		pDO->Commission = pTradingAccount->Commission;
		pDO->CloseProfit = pTradingAccount->CloseProfit;
		pDO->PositionProfit = pTradingAccount->PositionProfit;
		pDO->Balance = pTradingAccount->Balance;
		pDO->Available = pTradingAccount->Available;
		pDO->WithdrawQuota = pTradingAccount->WithdrawQuota;
		pDO->Reserve = pTradingAccount->Reserve;
		pDO->TradingDay = std::atoi(pTradingAccount->TradingDay);
		pDO->SettlementID = pTradingAccount->SettlementID;
		pDO->Credit = pTradingAccount->Credit;
		pDO->Mortgage = pTradingAccount->Mortgage;
		pDO->ExchangeMargin = pTradingAccount->ExchangeMargin;
		pDO->DeliveryMargin = pTradingAccount->DeliveryMargin;
		pDO->ExchangeDeliveryMargin = pTradingAccount->ExchangeDeliveryMargin;
		pDO->ReserveBalance = pTradingAccount->Reserve;
		pDO->RiskRatio = pDO->Balance > 0 ? pDO->CurrMargin / pDO->Balance : 0;

		std::string sysUserId = pTradingAccount->AccountID;
		//sysUserId += pTradingAccount->AccountID;

		std::set<std::string> userSet;
		_sharedSystemSessionHub->foreach(sysUserId, [&sysUserId, &accountInfo_ptr, &userSet, nRequestID, this](const IMessageSession_Ptr& session_ptr)
		{
			auto& userId = session_ptr->getUserInfo().getUserId();

			if (userSet.find(userId) == userSet.end())
			{
				userSet.emplace(userId);
				if (auto positionCtx = GetUserPositionContext()->GetPortfolioPositionsPnLByUser(userId))
				{
					double userBalance = 0;
					for (auto pair : positionCtx.map()->lock_table())
					{
						for (auto cpair : pair.second.map()->lock_table())
						{
							auto pnl = cpair.second.get();
							auto sellBalance = (pnl->SellAvgPrice() - pnl->LastPrice) * pnl->SellPosition();
							if (!std::isnan(sellBalance))
								userBalance += sellBalance;
							auto buyBalance = (pnl->LastPrice - pnl->BuyAvgPrice()) * pnl->BuyPosition();
							if (!std::isnan(buyBalance))
								userBalance += buyBalance;
						}
					}

					accountInfo_ptr->UserBalance = userBalance;
				}

				UpdateAccountInfo(userId, *accountInfo_ptr);
			}
			else
			{
				accountInfo_ptr = GetAccountInfo(userId);
			}

			SendDataObject(session_ptr, MSG_ID_QUERY_ACCOUNT_INFO, nRequestID, accountInfo_ptr);
		});
	}
}

