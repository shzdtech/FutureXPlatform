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
#include "../bizutility/ExchangeRouterTable.h"
#include "../bizutility/PositionPortfolioMap.h"

#include "../databaseop/VersionDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/TradeDAO.h"
#include "../databaseop/PositionDAO.h"
#include "../ordermanager/OrderSeqGen.h"
#include "../ordermanager/OrderPortfolioCache.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Purpose:    Implementation of CTPTradeWorkerProcessor::CTPTradeWorkerProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::CTPTradeWorkerProcessor(IServerContext* pServerCtx,
	const IUserPositionContext_Ptr& positionCtx)
	: _userSessionCtn_Ptr(SessionContainer<std::string>::NewInstancePtr())
{
	_loadPositionFromDB = false;

	_serverCtx = pServerCtx;

	_userPositionCtx_Ptr = positionCtx ? positionCtx : std::make_shared<UserPositionContext>();

	std::string brokerid;
	if (!_serverCtx->getConfigVal(CTP_TRADER_BROKERID, brokerid))
		SysParam::TryGet(CTP_TRADER_BROKERID, brokerid);
	_systemUser.setBrokerId(brokerid);

	std::string userid;
	if (!_serverCtx->getConfigVal(CTP_TRADER_USERID, userid))
		SysParam::TryGet(CTP_TRADER_USERID, userid);
	_systemUser.setInvestorId(userid);
	_systemUser.setUserId(userid);

	std::string pwd;
	if (!_serverCtx->getConfigVal(CTP_TRADER_PASSWORD, pwd))
		SysParam::TryGet(CTP_TRADER_PASSWORD, pwd);
	_systemUser.setPassword(pwd);

	std::string address;
	if (!_serverCtx->getConfigVal(CTP_TRADER_SERVER, address))
	{
		ExchangeRouterTable::TryFind(_systemUser.getBrokerId() + ':' + ExchangeRouterTable::TARGET_TD, address);
		if (address.empty() && !_serverCtx->getConfigVal(CTP_TRADER_SERVER, address))
			SysParam::TryGet(CTP_TRADER_SERVER, address);
	}
	_systemUser.setServer(address);


	std::string initPosFromDB;
	if (SysParam::TryGet(SYNC_POSITION_FROMDB, initPosFromDB))
	{
		_loadPositionFromDB = stringutility::compare(initPosFromDB, "true") == 0;
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Purpose:    Implementation of CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessor::~CTPTradeWorkerProcessor()
{
	_closing = true;

	if (_initializer.valid())
		_initializer.wait();

	LOG_DEBUG << __FUNCTION__;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessor::OnInit()
// Purpose:    Implementation of CTPTradeWorkerProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPTradeWorkerProcessor::Initialize(IServerContext* pServerCtx)
{
	std::string productTypes;
	if (getServerContext()->getConfigVal("product_type", productTypes))
	{
		std::vector<std::string> productVec;
		stringutility::split(productTypes, productVec);

		for (auto& strProductType : productVec)
			_productTypes.emplace((ProductType)std::stoi(strProductType));
	}
	else
	{
		for (int i = 0; i < ProductType::PRODUCT_UPPERBOUND; i++)
			_productTypes.emplace((ProductType)i);
	}

	LoadDataAsync();
}

int CTPTradeWorkerProcessor::RequestData(void)
{
	int ret = -1;

	if (HasLogged())
	{
		LOG_INFO << _serverCtx->getServerUri() << ": Try preloading data...";
		auto trdAPI = _rawAPI->TdAPI;

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

		//std::this_thread::sleep_for(delay);
		//CThostFtdcQryTradingAccountField reqaccount{};
		//ret = trdAPI->ReqQryTradingAccount(&reqaccount, 0);

		if (!(DataLoadMask & INSTRUMENT_DATA_LOADED))
		{
			CThostFtdcQryInstrumentField reqinstr{};
			ret = trdAPI->ReqQryInstrument(&reqinstr, 0);
		}

		if (!(DataLoadMask & POSITION_DATA_LOADED))
		{
			std::this_thread::sleep_for(DefaultQueryTime);
			CThostFtdcQryInvestorPositionField reqposition{};
			ret = trdAPI->ReqQryInvestorPosition(&reqposition, 0);
		}

		//if (!(DataLoadMask & EXCHANGE_DATA_LOADED))
		//{
		//	std::this_thread::sleep_for(DefaultQueryTime);
		//	CThostFtdcQryExchangeField reqexchange{};
		//	ret = trdAPI->ReqQryExchange(&reqexchange, 0);
		//	if (ret == 0)
		//		DataLoadMask |= EXCHANGE_DATA_LOADED;
		//	else
		//		return ret;
		//}

		//if (!(DataLoadMask & ORDER_DATA_LOADED))
		//{
		//	std::this_thread::sleep_for(DefaultQueryTime);
		//	CThostFtdcQryOrderField reqorder{};
		//	ret = trdAPI->ReqQryOrder(&reqorder, 0);
		//	if (ret == 0)
		//		DataLoadMask |= ORDER_DATA_LOADED;
		//	else
		//		return ret;
		//}

		//if (!(DataLoadMask & TRADE_DATA_LOADED))
		//{
		//	std::this_thread::sleep_for(DefaultQueryTime);
		//	CThostFtdcQryTradeField reqtrd{};
		//	ret = trdAPI->ReqQryTrade(&reqtrd, 0);
		//	if (ret == 0)
		//		DataLoadMask |= TRADE_DATA_LOADED;
		//	else
		//		return ret;
		//}
	}

	return ret;
}

int CTPTradeWorkerProcessor::LoginSystemUser(void)
{
	CThostFtdcReqUserLoginField req{};
	std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
	std::strncpy(req.UserID, _systemUser.getInvestorId().data(), sizeof(req.UserID));
	std::strncpy(req.Password, _systemUser.getPassword().data(), sizeof(req.Password));
	return _rawAPI->TdAPI->ReqUserLogin(&req, AppContext::GenNextSeq());
}

int CTPTradeWorkerProcessor::LoginSystemUserIfNeed(void)
{
	int ret = 0;

	if (!_isLogged || !_isConnected)
	{
		_isLogged = false;

		std::string address(_systemUser.getServer());
		CreateCTPAPI(_systemUser.getInvestorId(), address);

		ret = LoginSystemUser();
		if (ret == -1)
		{
			if (ExchangeRouterTable::TryFind(_systemUser.getBrokerId() + ':' + ExchangeRouterTable::TARGET_TD_AM, address))
			{
				CreateCTPAPI(_systemUser.getInvestorId(), address);
				ret = LoginSystemUser();
			}
		}

		if (ret == 0)
		{
			LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId()
				<< " has connected to trading server at: " << address;
		}
		else
		{
			LOG_WARN << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId()
				<< " cannot connect to trading server at: " << address;
		}
	}

	return ret;
}

int CTPTradeWorkerProcessor::LoadDataAsync(void)
{
	std::string logout_dataloaded;
	if (getServerContext()->getConfigVal("load_contract_db", logout_dataloaded) &&
		stringutility::compare(logout_dataloaded.data(), "true") == 0)
	{
		for (auto productType : _productTypes)
		{
			if (auto vectPtr = ContractDAO::FindContractByProductType(productType))
			{
				InstrumentCache& cache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);
				for (auto& contract : *vectPtr)
					cache.Add(contract);
			}
		}
	}

	bool loaded = DataLoadMask & (POSITION_DATA_LOADED | INSTRUMENT_DATA_LOADED);

	if (!loaded)
	{
		_initializer = std::async(std::launch::async,
			[this]() {
			auto millsec = std::chrono::milliseconds(500);
			bool loaded = DataLoadMask & (POSITION_DATA_LOADED | INSTRUMENT_DATA_LOADED);
			while (!_closing && !loaded)
			{
				if (!CTPUtility::HasReturnError(LoginSystemUserIfNeed()))
				{
					std::this_thread::sleep_for(std::chrono::seconds(3)); //wait 3 secs for login CTP server
					RequestData();
				}

				for (int cum_ms = 0; cum_ms < RetryInterval && !loaded; cum_ms += millsec.count())
				{
					std::this_thread::sleep_for(millsec);
					loaded = DataLoadMask & (POSITION_DATA_LOADED | INSTRUMENT_DATA_LOADED);
				}
			}

			if (loaded)
				OnDataLoaded();
		});
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
		std::strncpy(logout.BrokerID, _systemUser.getBrokerId().data(), sizeof(logout.BrokerID));
		std::strncpy(logout.UserID, _systemUser.getInvestorId().data(), sizeof(logout.UserID));
		_rawAPI->TdAPI->ReqUserLogout(&logout, 0);
		_isLogged = false;

		LOG_INFO << getServerContext()->getServerUri() << ": System user has logout.";
	}

	LOG_FLUSH;
}

///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CTPTradeWorkerProcessor::OnFrontConnected()
{
	CTPTradeProcessor::OnFrontConnected();

	auto loginTime = _systemUser.getLoginTime();
	auto now = std::time(nullptr);
	if (_isLogged && (now - loginTime) > 600)
	{
		LoginSystemUser();
	}
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

		LoadPositonFromDatabase(_systemUser.getInvestorId(), std::to_string(_systemUser.getTradingDay()));

		CThostFtdcSettlementInfoConfirmField reqsettle{};
		std::strncpy(reqsettle.BrokerID, _systemUser.getBrokerId().data(), sizeof(reqsettle.BrokerID));
		std::strncpy(reqsettle.InvestorID, _systemUser.getInvestorId().data(), sizeof(reqsettle.InvestorID));
		_rawAPI->TdAPI->ReqSettlementInfoConfirm(&reqsettle, 0);

		_isLogged = true;

		LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId() << " has logged on trading server.";
		LOG_FLUSH;
	}
}

void CTPTradeWorkerProcessor::LoadPositonFromDatabase(const std::string& sysuserid, const std::string& tradingday)
{
	if (!(DataLoadMask & PORTFOLIO_DATA_LOADED))
	{
		try
		{
			if (auto positionVec_ptr = PositionDAO::QueryLastDayPosition(sysuserid, tradingday))
			{
				for (auto it : *positionVec_ptr)
				{
					_ydDBPositions.emplace(std::make_pair(it.InstrumentID(), it.Direction), it.YdInitPosition);
					if (it.ExchangeID() == EXCHANGE_SHFE)
					{
						GetUserPositionContext()->UpsertPosition(it.UserID(), it, true, false);
					}
					else
					{
						GetUserPositionContext()->UpsertPosition(it.UserID(), it, false, true);
					}
				}
			}

			if (auto trades = TradeDAO::QueryExchangeTrade(sysuserid, "", tradingday, "9999-01-01"))
			{
				for (auto& trade : *trades)
				{
					if (!trade.PortfolioID().empty())
						OrderPortfolioCache::Insert(trade.OrderSysID, trade);

					UpdatePosition(std::make_shared<TradeRecordDO>(trade));
				}
			}

			DataLoadMask |= PORTFOLIO_DATA_LOADED;
		}
		catch (std::exception& ex)
		{
			LOG_ERROR << ex.what();
		}
	}
}

void CTPTradeWorkerProcessor::OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	try
	{
		ProcessResponseMacro(this, MSG_ID_QUERY_INSTRUMENT, nRequestID, pInstrument, pRspInfo, &nRequestID, &bIsLast);

		if (bIsLast)
		{
			DataLoadMask |= INSTRUMENT_DATA_LOADED;

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
			if (orderptr->ErrorCode)
			{
				OrderPortfolioCache::Remove(OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID));
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
				OrderPortfolioCache::Remove(OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID));
			}
			orderptr->HasMore = !bIsLast;
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

OrderDO_Ptr CTPTradeWorkerProcessor::RefineOrder(CThostFtdcOrderField *pOrder)
{
	// update orders
	auto orderSysId = CTPUtility::ToUInt64(pOrder->OrderSysID);
	OrderDO_Ptr ctxOrder = _userOrderCtx.FindOrder(orderSysId);
	auto orderptr = CTPUtility::ParseRawOrder(pOrder, ctxOrder);

	if (orderptr->PortfolioID().empty())
	{
		PortfolioKey portfolio;
		if (OrderPortfolioCache::Find(OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID), portfolio))
		{
			if (orderptr->IsSystemUserId())
			{
				orderptr->SetUserID(portfolio.UserID());
			}
			orderptr->SetPortfolioID(portfolio.PortfolioID());
		}
		else if (orderSysId && OrderPortfolioCache::Find(orderSysId, portfolio))
		{
			if (orderptr->IsSystemUserId())
			{
				orderptr->SetUserID(portfolio.UserID());
			}
			orderptr->SetPortfolioID(portfolio.PortfolioID());
		}
	}

	if (orderSysId)
	{
		_userOrderCtx.UpsertOrder(orderSysId, orderptr);
	}

	return orderptr;
}


void CTPTradeWorkerProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		if (auto trdDO_Ptr = RefineTrade(pTrade))
		{
			DispatchUserMessage(MSG_ID_TRADE_RTN, 0, trdDO_Ptr->UserID(), trdDO_Ptr);

			UpdatePosition(trdDO_Ptr);
		}
	}
}

///请求查询投资者持仓响应
void CTPTradeWorkerProcessor::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast)
		DataLoadMask |= CTPTradeProcessor::POSITION_DATA_LOADED;

	if (pInvestorPosition)
	{
		if (auto position_ptr = CTPUtility::ParseRawPosition(pInvestorPosition))

			if (position_ptr->ExchangeID() == EXCHANGE_SHFE)
			{
				if (pInvestorPosition->PositionDate == THOST_FTDC_PSD_History)
				{
					_ydSysPositions.emplace(std::make_pair(position_ptr->InstrumentID(), position_ptr->Direction), position_ptr->YdInitPosition);
				}
			}
			else
				_ydSysPositions.emplace(std::make_pair(position_ptr->InstrumentID(), position_ptr->Direction), position_ptr->YdInitPosition);
	}
}


TradeRecordDO_Ptr CTPTradeWorkerProcessor::RefineTrade(CThostFtdcTradeField * pTrade)
{
	TradeRecordDO_Ptr trdDO_Ptr = CTPUtility::ParseRawTrade(pTrade);

	if (trdDO_Ptr)
	{
		PortfolioKey portfolio;
		if (OrderPortfolioCache::Find(trdDO_Ptr->OrderSysID, portfolio))
		{
			if (trdDO_Ptr->IsSystemUserId())
			{
				trdDO_Ptr->SetUserID(portfolio.UserID());
			}
			trdDO_Ptr->SetPortfolioID(portfolio.PortfolioID());
		}
		else if (auto order_ptr = _userOrderCtx.FindOrder(trdDO_Ptr->OrderSysID))
		{
			if (!order_ptr->PortfolioID().empty())
			{
				if (trdDO_Ptr->IsSystemUserId())
				{
					trdDO_Ptr->SetUserID(order_ptr->UserID());
				}
				trdDO_Ptr->SetPortfolioID(order_ptr->PortfolioID());
			}
			else if (OrderPortfolioCache::Find(OrderSeqGen::GetOrderID(order_ptr->OrderID, order_ptr->SessionID), portfolio))
			{
				if (trdDO_Ptr->IsSystemUserId())
				{
					trdDO_Ptr->SetUserID(portfolio.UserID());
				}
				trdDO_Ptr->SetPortfolioID(portfolio.PortfolioID());
			}
		}
		else if (OrderPortfolioCache::Find(OrderSeqGen::GetOrderID(trdDO_Ptr->OrderID, _systemUser.getSessionId()), portfolio))
		{
			if (trdDO_Ptr->IsSystemUserId())
			{
				trdDO_Ptr->SetUserID(portfolio.UserID());
			}
			trdDO_Ptr->SetPortfolioID(portfolio.PortfolioID());
		}

		//if (trdDO_Ptr->PortfolioID().empty())
		//{
		//	if (auto pPortfolioKey = PositionPortfolioMap::FindPortfolio(trdDO_Ptr->InstrumentID()))
		//	{
		//		trdDO_Ptr->SetUserID(pPortfolioKey->UserID());
		//		trdDO_Ptr->SetPortfolioID(pPortfolioKey->PortfolioID());
		//	}
		//}

		if (trdDO_Ptr->IsSystemUserId())
		{
			trdDO_Ptr->SetUserID(_systemUser.getUserId());
		}
	}

	return trdDO_Ptr;
}

void CTPTradeWorkerProcessor::UpdatePosition(const TradeRecordDO_Ptr& trdDO_Ptr)
{
	if (GetUserTradeContext().InsertTrade(trdDO_Ptr))
	{
		int multiplier = 1;
		if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(trdDO_Ptr->InstrumentID()))
		{
			multiplier = pContractInfo->VolumeMultiple;
		}
		else
		{
			InstrumentDO instDO;
			if (ContractDAO::FindContractById(*trdDO_Ptr, instDO))
			{
				multiplier = instDO.VolumeMultiple;
				ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).Add(instDO);
			}
		}

		auto position_ptr = GetUserPositionContext()->UpsertPosition(trdDO_Ptr->UserID(), trdDO_Ptr, multiplier, trdDO_Ptr->ExchangeID() != EXCHANGE_SHFE);
	}
}

bool CTPTradeWorkerProcessor::IsLoadPositionFromDB()
{
	return _loadPositionFromDB;
}

void CTPTradeWorkerProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	if (sessionPtr->getLoginTimeStamp() && _isLogged)
	{
		auto& userInfo = sessionPtr->getUserInfo();
		userInfo.setBrokerId(_systemUser.getBrokerId());
		userInfo.setInvestorId(_systemUser.getInvestorId());
		userInfo.setFrontId(_systemUser.getFrontId());
		userInfo.setSessionId(_systemUser.getSessionId());
		_userSessionCtn_Ptr->add(userInfo.getUserId(), sessionPtr);
	}
}

autofillmap<std::string, AccountInfoDO>& CTPTradeWorkerProcessor::GetAccountInfo(const std::string userId)
{
	return _accountInfoMap.getorfill(userId);
}

std::set<ExchangeDO>& CTPTradeWorkerProcessor::GetExchangeInfo()
{
	return _exchangeInfo_Set;
}

IUserPositionContext_Ptr& CTPTradeWorkerProcessor::GetUserPositionContext()
{
	return _userPositionCtx_Ptr;
}

UserTradeContext& CTPTradeWorkerProcessor::GetUserTradeContext()
{
	return _userTradeCtx;
}

UserOrderContext & CTPTradeWorkerProcessor::GetUserOrderContext(void)
{
	return _userOrderCtx;
}

std::set<ProductType>& CTPTradeWorkerProcessor::GetProductTypeToLoad()
{
	return _productTypes;
}

//UserOrderContext & CTPTradeWorkerProcessor::GetUserErrOrderContext(void)
//{
//	return _userErrOrderCtx;
//}

void CTPTradeWorkerProcessor::DispatchUserMessage(int msgId, int serialId, const std::string& userId,
	const dataobj_ptr& dataobj_ptr)
{
	_userSessionCtn_Ptr->foreach(userId, [msgId, serialId, dataobj_ptr, this](const IMessageSession_Ptr& session_ptr)
	{ SendDataObject(session_ptr, msgId, serialId, dataobj_ptr); });
}

const IUserInfo& CTPTradeWorkerProcessor::GetSystemUser()
{
	return _systemUser;
}

int CTPTradeWorkerProcessor::ComparePosition(autofillmap<std::pair<std::string, PositionDirectionType>, std::pair<int, int>>& positions)
{
	for (auto pair : _ydDBPositions)
	{
		auto& position = positions.getorfill(pair.first);
		position.first = pair.second;
		if (auto pSysPos = _ydSysPositions.tryfind(pair.first))
		{
			position.second = *pSysPos;
		}
	}

	for (auto pair : _ydSysPositions)
	{
		auto& position = positions.getorfill(pair.first);
		position.second = pair.second;
		if (auto pDbPos = _ydDBPositions.tryfind(pair.first))
		{
			position.first = *pDbPos;
		}
	}

	return positions.size();
}
