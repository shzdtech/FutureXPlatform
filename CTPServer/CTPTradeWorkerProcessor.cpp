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

CTPTradeWorkerProcessor::CTPTradeWorkerProcessor(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx)
	: _accountInfoMap(4), _ydDBPositions(4), _ydSysPositions(4)
{
	_serverCtx = pServerCtx;

	_userPositionCtx_Ptr = positionCtx ? positionCtx : std::make_shared<UserPositionContext>();

	std::string defaultCfg;
	_serverCtx->getConfigVal(ExchangeRouterTable::TARGET_TD, defaultCfg);
	ExchangeRouterDO exDO;
	if (ExchangeRouterTable::TryFind(defaultCfg, exDO))
	{
		_systemUser.setBrokerId(exDO.BrokeID);
		_systemUser.setUserId(exDO.UserID);
		_systemUser.setInvestorId(exDO.UserID);
		_systemUser.setPassword(exDO.Password);
		_systemUser.setServer(exDO.Address);
		_authCode = exDO.AuthCode;
		_productInfo = exDO.ProductInfo;
	}

	std::string initPosFromDB;
	if (_serverCtx->getConfigVal(SYNC_POSITION_FROMDB, initPosFromDB))
	{
		_loadPositionFromDB = stringutility::compare(initPosFromDB, "true") == 0;
	}

	std::string saveTrade;
	if (_serverCtx->getConfigVal(SYNC_POSITION_FROMDB, saveTrade))
	{
		_logTrades = stringutility::compare(saveTrade, "true") == 0;
	}

	if (_logTrades)
		_tradeDBSerializer = std::move(std::thread(&CTPTradeWorkerProcessor::LogTrade, this));
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
	{
		_initializer.wait();
	}

	if (_tradeDBSerializer.joinable())
		_tradeDBSerializer.join();

	LOG_DEBUG << __FUNCTION__;
}

void CTPTradeWorkerProcessor::LogTrade()
{
	std::vector<TradeRecordDO_Ptr> retryList;
	while (!_closing)
	{
		for (auto trade : retryList)
		{
			_tradeQueue.push(trade);
		}

		bool hasError = false;
		TradeRecordDO_Ptr trade_ptr;
		if (_tradeQueue.pop(trade_ptr))
		{
			if (trade_ptr)
			{
				try
				{
					TradeDAO::SaveExchangeTrade(*trade_ptr);
				}
				catch (std::exception& ex)
				{
					hasError = true;
					LOG_ERROR << ex.what();
				}
				catch (...)
				{
					hasError = true;
				}

				if (hasError)
				{
					retryList.push_back(trade_ptr);
				}
			}
		}

		if (hasError || _tradeQueue.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
}

bool CTPTradeWorkerProcessor::IsLoadPositionFromDB()
{
	return _loadPositionFromDB;
}

bool CTPTradeWorkerProcessor::IsSaveTrade()
{
	return _logTrades;
}

void CTPTradeWorkerProcessor::OnNewManualTrade(const TradeRecordDO & tradeDO)
{
	GetUserTradeContext().InsertTrade(tradeDO);

	auto tradeDO_Ptr = std::make_shared<TradeRecordDO>(tradeDO);

	DispatchUserMessage(MSG_ID_TRADE_RTN, 0, tradeDO.UserID(), tradeDO_Ptr);

	int multiplier = 1;
	if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(tradeDO.InstrumentID()))
	{
		multiplier = pContractInfo->VolumeMultiple;
	}

	if (auto userPosition_Ptr = GetUserPositionContext()->UpsertPosition(tradeDO.UserID(), tradeDO_Ptr, multiplier))
	{
		DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, tradeDO_Ptr->UserID(), userPosition_Ptr);
	}
}

void CTPTradeWorkerProcessor::OnUpdateManualPosition(const UserPositionExDO & positionDO)
{
	if (auto userPosition_Ptr = GetUserPositionContext()->UpsertPosition(positionDO.UserID(), positionDO, true, false))
	{
		if (auto position_ptr = FindDBYdPostion(positionDO.UserID(), positionDO.InstrumentID(), positionDO.PortfolioID(), positionDO.Direction))
		{
			position_ptr->YdInitPosition = positionDO.YdInitPosition;
		}

		DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, userPosition_Ptr->UserID(), userPosition_Ptr);
	}
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
		auto trdAPI = _rawAPI->TdAPIProxy();

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

		if (!(DataLoadMask & INSTRUMENT_DATA_LOADED))
		{
			CThostFtdcQryInstrumentField reqinstr{};
			ret = trdAPI->get()->ReqQryInstrument(&reqinstr, 0);
		}

		//std::this_thread::sleep_for(delay);
		//CThostFtdcQryTradingAccountField reqaccount{};
		//ret = trdAPI->ReqQryTradingAccount(&reqaccount, 0);

		if (!(DataLoadMask & POSITION_DATA_LOADED))
		{
			std::this_thread::sleep_for(DefaultQueryTime);
			CThostFtdcQryInvestorPositionField reqposition{};
			ret = trdAPI->get()->ReqQryInvestorPosition(&reqposition, 0);
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
	int ret = 0;

	if (!_authCode.empty())
	{
		CThostFtdcReqAuthenticateField reqAuth{};
		std::strncpy(reqAuth.BrokerID, _systemUser.getBrokerId().data(), sizeof(reqAuth.BrokerID));
		std::strncpy(reqAuth.UserID, _systemUser.getUserId().data(), sizeof(reqAuth.UserID));
		std::strncpy(reqAuth.AuthCode, _authCode.data(), sizeof(reqAuth.AuthCode));
		std::strncpy(reqAuth.UserProductInfo, _productInfo.data(), sizeof(reqAuth.UserProductInfo));
		ret = _rawAPI->TdAPIProxy()->get()->ReqAuthenticate(&reqAuth, 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (ret == 0)
	{
		CThostFtdcReqUserLoginField req{};
		std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
		std::strncpy(req.UserID, _systemUser.getInvestorId().data(), sizeof(req.UserID));
		std::strncpy(req.Password, _systemUser.getPassword().data(), sizeof(req.Password));
		std::strncpy(req.UserProductInfo, _productInfo.data(), sizeof(req.UserProductInfo));

		ret = _rawAPI->TdAPIProxy()->get()->ReqUserLogin(&req, 0);
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
		CreateCTPAPI(_systemUser.getInvestorId(), address);
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
				CreateCTPAPI(_systemUser.getInvestorId(), exDO.Address);
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

int CTPTradeWorkerProcessor::LoadContractFromDB()
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

	DataLoadMask |= INSTRUMENT_DATA_LOADED;

	return 0;
}

int CTPTradeWorkerProcessor::LoadDataAsync(void)
{
	std::string logout_dataloaded;
	if (getServerContext()->getConfigVal("load_contract_db", logout_dataloaded) &&
		stringutility::compare(logout_dataloaded.data(), "true") == 0)
	{
		LoadContractFromDB();
	}

	bool loaded = DataLoadMask & (INSTRUMENT_DATA_LOADED);

	if (!loaded)
	{
		_initializer = std::async(std::launch::async,
			[this]() {
			auto millsec = std::chrono::milliseconds(500);
			bool loaded = DataLoadMask & (INSTRUMENT_DATA_LOADED);
			int i = 0;
			while (!_closing && !loaded && i++ < RetryTimes)
			{
				if (!CTPUtility::HasReturnError(LoginSystemUserIfNeed()))
				{
					std::this_thread::sleep_for(std::chrono::seconds(3)); //wait 3 secs for login CTP server
					RequestData();
				}

				for (int cum_ms = 0; cum_ms < RetryInterval && !_closing && !loaded; cum_ms += millsec.count())
				{
					std::this_thread::sleep_for(millsec);
					loaded = DataLoadMask & (INSTRUMENT_DATA_LOADED);
				}
			}

			if (!loaded)
			{
				LoadContractFromDB();
			}

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
		_rawAPI->TdAPIProxy()->get()->ReqUserLogout(&logout, 0);
		_isLogged = false;

		LOG_INFO << getServerContext()->getServerUri() << ": System user has logout.";
	}

	LOG_FLUSH;
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

	//_rawAPI->TdAPIProxy()->get()->ReqUserLogin(&req, 0);
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
		_rawAPI->TdAPIProxy()->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

		_isLogged = true;

		LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId() << " has logged on trading server.";
		LOG_FLUSH;
	}
}

void CTPTradeWorkerProcessor::LoadYdPositonFromDatabase(const std::string& sysuserid, const std::string& tradingday)
{
	if (auto positionVec_ptr = PositionDAO::QueryLastDayPosition(sysuserid, tradingday))
	{
		for (auto it : *positionVec_ptr)
		{
			Position_HashMap posMap;
			if (!_ydDBPositions.find(it.UserID(), posMap))
			{
				_ydDBPositions.insert(it.UserID(), std::move(Position_HashMap(4)));
				_ydDBPositions.find(it.UserID(), posMap);
			}
			posMap.map()->insert_or_assign(std::make_tuple(it.InstrumentID(), it.PortfolioID(), it.Direction), std::make_shared<UserPositionExDO>(it));
		}
	}
}

void CTPTradeWorkerProcessor::LoadPositonFromDatabase(const std::string& sysuserid, const std::string& tradingday)
{
	try
	{
		if (auto positionVec_ptr = PositionDAO::QueryLastDayPosition(sysuserid, tradingday))
		{
			for (auto it : *positionVec_ptr)
			{
				Position_HashMap posMap;
				if (!_ydDBPositions.find(it.UserID(), posMap))
				{
					_ydDBPositions.insert(it.UserID(), std::move(Position_HashMap(4)));
					_ydDBPositions.find(it.UserID(), posMap);
				}
				posMap.map()->insert_or_assign(std::make_tuple(it.InstrumentID(), it.PortfolioID(), it.Direction), std::make_shared<UserPositionExDO>(it));
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
	}
	catch (std::exception& ex)
	{
		LOG_ERROR << ex.what();
	}
}

UserPositionExDO_Ptr CTPTradeWorkerProcessor::FindDBYdPostion(const std::string& userid,
	const std::string & contract, const std::string & portfolio, PositionDirectionType direction)
{
	UserPositionExDO_Ptr ret;
	Position_HashMap posMap;
	if (_ydDBPositions.find(userid, posMap))
	{
		posMap.map()->find(std::make_tuple(contract, portfolio, direction), ret);
	}
	return ret;
}

UserPositionExDO_Ptr CTPTradeWorkerProcessor::FindSysYdPostion(const std::string& userid,
	const std::string & contract, const std::string & portfolio, PositionDirectionType direction)
{
	UserPositionExDO_Ptr ret;
	Position_HashMap posMap;
	if (_ydSysPositions.find(userid, posMap))
	{
		posMap.map()->find(std::make_tuple(contract, portfolio, direction), ret);
	}
	return ret;
}

void CTPTradeWorkerProcessor::PushToLogQueue(const TradeRecordDO_Ptr & tradeDO_Ptr)
{
	if (_logTrades)
		_tradeQueue.push(tradeDO_Ptr);
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

	if (orderptr->PortfolioID().empty())
	{
		if (auto pPortfolioKey = PositionPortfolioMap::FindPortfolio(orderptr->InstrumentID()))
		{
			orderptr->SetPortfolioID(pPortfolioKey->PortfolioID());
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
		_tradeCnt++;

		if (auto trdDO_Ptr = RefineTrade(pTrade))
		{
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
	if (bIsLast)
		DataLoadMask |= CTPProcessor::POSITION_DATA_LOADED;

	if (pInvestorPosition)
	{
		if (auto position_ptr = CTPUtility::ParseRawPosition(pInvestorPosition))
		{
			UpdateYdPosition(GetSystemUser().getUserId(), position_ptr);
			DispatchMessageForAll(MSG_ID_EXCHANGE_POSITION_UPDATED, 0, position_ptr);
		}
	}
}

void CTPTradeWorkerProcessor::UpdateYdPosition(const std::string& userId, const UserPositionExDO_Ptr& position_ptr)
{
	if (position_ptr->ExchangeID() != EXCHANGE_SHFE || position_ptr->PositionDateFlag == PositionDateFlagType::PSD_HISTORY)
	{
		Position_HashMap posMap;
		if (!_ydSysPositions.find(userId, posMap))
		{
			_ydSysPositions.insert(userId, std::move(Position_HashMap(4)));
			_ydSysPositions.find(userId, posMap);

			posMap.map()->insert_or_assign(std::make_tuple(position_ptr->InstrumentID(), position_ptr->PortfolioID(), position_ptr->Direction), position_ptr);
		}
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

		if (trdDO_Ptr->PortfolioID().empty())
		{
			if (auto pPortfolioKey = PositionPortfolioMap::FindPortfolio(trdDO_Ptr->InstrumentID()))
			{
				trdDO_Ptr->SetUserID(pPortfolioKey->UserID());
				trdDO_Ptr->SetPortfolioID(pPortfolioKey->PortfolioID());
			}
		}

		if (trdDO_Ptr->IsSystemUserId())
		{
			trdDO_Ptr->SetUserID(_systemUser.getUserId());
		}
	}

	return trdDO_Ptr;
}

UserPositionExDO_Ptr CTPTradeWorkerProcessor::UpdatePosition(const TradeRecordDO_Ptr& trdDO_Ptr)
{
	UserPositionExDO_Ptr ret;

	if (GetUserTradeContext().InsertTrade(trdDO_Ptr))
	{
		int multiplier = 1;
		if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(trdDO_Ptr->InstrumentID()))
		{
			multiplier = pContractInfo->VolumeMultiple;
		}

		ret = GetUserPositionContext()->UpsertPosition(trdDO_Ptr->UserID(), trdDO_Ptr, multiplier, trdDO_Ptr->ExchangeID() != EXCHANGE_SHFE);
	}

	return ret;
}

AccountInfoDO_Ptr CTPTradeWorkerProcessor::GetAccountInfo(const std::string& userId)
{
	AccountInfoDO_Ptr ret;
	_accountInfoMap.find(userId, ret);
	return ret;
}

void CTPTradeWorkerProcessor::UpdateAccountInfo(const std::string& userId, const AccountInfoDO_Ptr & accoutInfo)
{
	_accountInfoMap.upsert(userId, [&accoutInfo](AccountInfoDO_Ptr & accoutInfo_ptr) {accoutInfo_ptr = accoutInfo; }, accoutInfo);
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

void CTPTradeWorkerProcessor::DispatchUserMessage(int msgId, int serialId, const std::string& userId,
	const dataobj_ptr& dataobj_ptr)
{
	_userSessionCtn_Ptr->foreach(userId, [msgId, serialId, dataobj_ptr, this](const IMessageSession_Ptr& session_ptr)
	{ SendDataObject(session_ptr, msgId, serialId, dataobj_ptr); });
}

void CTPTradeWorkerProcessor::DispatchMessageForAll(int msgId, int serialId, const dataobj_ptr& dataobj_ptr)
{
	_userSessionCtn_Ptr->forall([msgId, serialId, dataobj_ptr, this](const IMessageSession_Ptr& session_ptr)
	{ SendDataObject(session_ptr, msgId, serialId, dataobj_ptr); });
}

int CTPTradeWorkerProcessor::ComparePosition(const std::string& userId, autofillmap<std::tuple<std::string, std::string, PositionDirectionType>, std::pair<int, int>>& positions)
{
	Position_HashMap posDBMap;
	if (_ydDBPositions.find(userId, posDBMap))
	{
		for (auto& pair : posDBMap.map()->lock_table())
		{
			positions.emplace(pair.first, std::make_pair(pair.second->YdInitPosition, 0));
		}
	}

	Position_HashMap posSysMap;
	if (_ydSysPositions.find(userId, posSysMap))
	{
		for (auto& pair : posSysMap.map()->lock_table())
		{
			if (auto pPosition = positions.tryfind(pair.first))
			{
				pPosition->second = pair.second->YdInitPosition;
			}
			else
			{
				bool found = false;

				std::string contract, portfolio;
				PositionDirectionType direction;
				std::tie(contract, portfolio, direction) = pair.first;
				if (portfolio.empty())
				{
					if (!posDBMap.empty())
					{
						for (auto& dbpair : posDBMap.map()->lock_table())
						{
							std::string dbcontract, dbportfolio;
							PositionDirectionType dbdirection;
							std::tie(dbcontract, dbportfolio, dbdirection) = dbpair.first;

							if (dbcontract == contract && dbdirection == direction)
							{
								if (auto pPosition = positions.tryfind(dbpair.first))
								{
									pPosition->second = pair.second->YdInitPosition;
									found = true;
									break;
								}
							}
						}
					}

					if (!found)
					{
						positions.emplace(pair.first, std::make_pair(0, pair.second->YdInitPosition));
					}
				}
			}
		}
	}

	return positions.size();
}
