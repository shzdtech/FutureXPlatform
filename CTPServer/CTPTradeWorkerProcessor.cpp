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
	: _accountInfoMap(4), _ydDBPositions(4), _ydSysPositions(4), _logTrades(false), _isQueryAccount(true), _loadPositionFromDB(false), _pMktDataMap(nullptr)
{
	_serverCtx = pServerCtx;

	_userPositionCtx_Ptr = positionCtx ? positionCtx : std::make_shared<PortfolioPositionContext>();

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
	if (SysParam::TryGet(SYNC_POSITION_FROMDB, initPosFromDB))
	{
		_loadPositionFromDB = stringutility::compare(initPosFromDB, "true") == 0;
	}

	std::string saveTrade;
	if (_serverCtx->getConfigVal(SAVE_TRADE, saveTrade))
	{
		_logTrades = stringutility::compare(saveTrade, "true") == 0;
	}

	if (_logTrades)
		_tradeDBSerializer = std::async(std::launch::async, &CTPTradeWorkerProcessor::LogTrade, this);

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

	if (_initializer.valid())
		_initializer.wait();

	if (_accountQuery.valid())
		_accountQuery.wait();

	if (_tradeDBSerializer.valid())
		_tradeDBSerializer.wait();

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

void CTPTradeWorkerProcessor::QueryAccountWorker()
{
	while (!_closing)
	{
		_userSessionCtn_Ptr->forall([this](const IMessageSession_Ptr& session_ptr)
		{
			CThostFtdcQryTradingAccountField req{};
			//std::strncpy(req.BrokerID, session_ptr->getUserInfo().getBrokerId().data(), sizeof(req.BrokerID));
			//std::strncpy(req.InvestorID, session_ptr->getUserInfo().getInvestorId.data(), sizeof(req.InvestorID));
			if (auto msgProc = session_ptr->LockMessageProcessor())
			{
				if (auto tdApi = ((CTPRawAPI*)msgProc->getRawAPI())->TdAPIProxy())
					tdApi->get()->ReqQryTradingAccount(&req, 0);
			}
		});

		std::this_thread::sleep_for(std::chrono::seconds(2));
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

int CTPTradeWorkerProcessor::CheckRisk(const OrderRequestDO & orderReq)
{
	int ret = 0;
	if (orderReq.OpenClose == OrderOpenCloseType::OPEN && orderReq.TradingType != OrderTradingType::TRADINGTYPE_HEDGE)
	{
		if (auto modelKeyMap = RiskContext::Instance()->GetPreTradeUserModel(orderReq))
		{
			auto account_ptr = GetAccountInfo(orderReq.UserID());
			auto positionCtx = GetUserPositionContext();
			for (auto pair : modelKeyMap.map()->lock_table())
			{
				if (auto modelparams_ptr = ModelParamsCache::FindModel(pair.first))
				{
					if (auto model_ptr = RiskModelAlgorithmManager::Instance()->FindModel(modelparams_ptr->Model))
					{
						int action = model_ptr->CheckRisk(orderReq, *modelparams_ptr, positionCtx.get(), account_ptr.get());
						if (action != 0)
							ret = action;
					}
				}
			}
		}
		else
		{
			throw BizException(RiskModelParams::STOP_OPEN_ORDER, "User: " + orderReq.UserID() + " Portfolio: " + orderReq.PortfolioID() + " is not allowed for open order!");
		}
	}
	return ret;
}

void CTPTradeWorkerProcessor::OnNewManualTrade(const TradeRecordDO & tradeDO)
{
	GetUserTradeContext().InsertTrade(tradeDO.UserID(), tradeDO);

	auto tradeDO_Ptr = std::make_shared<TradeRecordDO>(tradeDO);

	DispatchUserMessage(MSG_ID_TRADE_RTN, 0, tradeDO.UserID(), tradeDO_Ptr);

	if (!GetUserPositionContext()->ContainsTrade(tradeDO.TradeID128()))
	{
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
}

void CTPTradeWorkerProcessor::OnUpdateManualPosition(const UserPositionExDO & positionDO)
{
	bool found = false;
	Position_HashMap positionMap;
	if (_ydDBPositions.find(positionDO.UserID(), positionMap))
	{
		std::vector<std::tuple<std::string, std::string, PositionDirectionType>> dbdelete;
		for (auto& dbpair : positionMap.map()->lock_table())
		{
			std::string dbcontract, dbportfolio;
			PositionDirectionType dbdirection;
			std::tie(dbcontract, dbportfolio, dbdirection) = dbpair.first;

			if (dbcontract == positionDO.InstrumentID() && dbdirection == positionDO.Direction)
			{
				if (dbportfolio == positionDO.PortfolioID())
				{
					dbpair.second->YdInitPosition = positionDO.YdInitPosition;
					found = true;
				}
				else
				{
					dbpair.second->YdInitPosition = 0;
					dbdelete.push_back(dbpair.first);
				}
			}
		}

		if (!found)
		{
			positionMap.map()->insert(std::make_tuple(positionDO.InstrumentID(), positionDO.PortfolioID(), positionDO.Direction),
				std::make_shared<UserPositionExDO>(positionDO));
		}

		for (auto& key : dbdelete)
		{
			positionMap.map()->erase(key);
		}
	}

	found = false;
	auto portfolioPosition = GetUserPositionContext()->GetPortfolioPositionsByUser(positionDO.UserID());
	if (!portfolioPosition.empty())
	{
		for (auto& pair : portfolioPosition.map()->lock_table())
		{
			auto& portfolio = pair.first;
			for (auto& pospair : pair.second.map()->lock_table())
			{
				if (pospair.second->InstrumentID() == positionDO.InstrumentID() && pospair.second->Direction == positionDO.Direction)
				{
					if (pospair.second->PortfolioID() == portfolio)
					{
						pospair.second->YdInitPosition = positionDO.YdInitPosition;
						found = true;
						if (auto positionPnL_ptr = GetUserPositionContext()->GetPositionPnL(positionDO.UserID(), positionDO.InstrumentID(), portfolio))
						{
							if (positionDO.Direction == PositionDirectionType::PD_LONG)
								positionPnL_ptr->YdBuyVolume = positionDO.YdInitPosition;
							else if (positionDO.Direction == PositionDirectionType::PD_SHORT)
								positionPnL_ptr->YdSellVolume = positionDO.YdInitPosition;
						}
					}
					else
					{
						pospair.second->YdInitPosition = 0;
						if (auto positionPnL_ptr = GetUserPositionContext()->GetPositionPnL(positionDO.UserID(), positionDO.InstrumentID(), portfolio))
						{
							if (positionDO.Direction == PositionDirectionType::PD_LONG)
								positionPnL_ptr->YdBuyVolume = 0;
							else if (positionDO.Direction == PositionDirectionType::PD_SHORT)
								positionPnL_ptr->YdSellVolume = 0;
						}
					}

					DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, positionDO.UserID(), pospair.second);
				}
			}
		}
	}

	if (!found)
	{
		if (auto position_ptr = GetUserPositionContext()->UpsertPosition(positionDO.UserID(), positionDO, true, false))
		{
			DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, positionDO.UserID(), position_ptr);
		}
	}
}

void CTPTradeWorkerProcessor::OnUpdateMarketData(const MarketDataDO & mdDO)
{
	_userSessionCtn_Ptr->forall([this, &mdDO](const IMessageSession_Ptr& msgSession)
	{
		GetUserPositionContext()->UpdatePnL(msgSession->getUserInfo().getUserId(), mdDO);
	});
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

		/*if (!(DataLoadMask & POSITION_DATA_LOADED))
		{
			std::this_thread::sleep_for(DefaultQueryTime);
			CThostFtdcQryInvestorPositionField reqposition{};
			ret = trdAPI->get()->ReqQryInvestorPosition(&reqposition, 0);
		}*/

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
		_rawAPI->TdAPIProxy()->get()->ReqSettlementInfoConfirm(&reqsettle, 0);

		_isLogged = true;

		LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId() << " has logged on trading server.";
		LOG_FLUSH;
	}
}

void CTPTradeWorkerProcessor::LoadPositonFromDatabase(const std::string & userId, const std::string& tradingday)
{
	try
	{
		auto mktDataMap = std::static_pointer_cast<MarketDataDOMap>(AppContext::GetData(STR_KEY_APP_MARKETDATA));
		if (auto positionVec_ptr = PositionDAO::QueryLastDayPosition(userId, tradingday))
		{
			for (auto it : *positionVec_ptr)
			{
				if (mktDataMap)
				{
					MarketDataDO mdo;
					if (mktDataMap->find(mdo.InstrumentID(), mdo))
					{
						it.LastPrice = mdo.LastPrice;
						it.PreSettlementPrice = mdo.PreSettlementPrice;
					}

					if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(it.InstrumentID()))
					{
						it.Multiplier = pContractInfo->VolumeMultiple;
					}
				}
				Position_HashMap posMap;
				if (!_ydDBPositions.find(userId, posMap))
				{
					_ydDBPositions.insert(userId, std::move(Position_HashMap(4)));
					_ydDBPositions.find(userId, posMap);
				}
				posMap.map()->insert_or_assign(std::make_tuple(it.InstrumentID(), it.PortfolioID(), it.Direction), std::make_shared<UserPositionExDO>(it));
				GetUserPositionContext()->UpsertPosition(userId, it, true, it.ExchangeID() != EXCHANGE_SHFE);
			}
		}

		if (auto trades = TradeDAO::QueryExchangeTrade(userId, "", tradingday, tradingday))
		{
			for (auto& trade : *trades)
			{
				if (!trade.PortfolioID().empty())
					OrderPortfolioCache::Insert(trade.OrderSysID, trade);

				auto tradePtr = std::make_shared<TradeRecordDO>(trade);

				GetUserTradeContext().InsertTrade(tradePtr->UserID(), tradePtr);

				UpdatePosition(tradePtr);
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
	const std::string & contract, PositionDirectionType direction)
{
	UserPositionExDO_Ptr ret;
	SysPosition_HashMap posMap;
	if (_ydSysPositions.find(userid, posMap))
	{
		posMap.map()->find(std::make_pair(contract, direction), ret);
	}
	return ret;
}

MarketDataDOMap * CTPTradeWorkerProcessor::GetMarketDataDOMap()
{
	if (!_pMktDataMap)
	{
		if (auto mktDataMap = std::static_pointer_cast<MarketDataDOMap>(AppContext::GetData(STR_KEY_APP_MARKETDATA)))
		{
			_pMktDataMap = mktDataMap.get();
		}
	}

	return _pMktDataMap;
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
		auto& userId = GetSystemUser().getUserId();

		if (auto position_ptr = CTPUtility::ParseRawPosition(pInvestorPosition, userId))
		{
			UpdateSysYdPosition(userId, position_ptr);
			// DispatchMessageForAll(MSG_ID_EXCHANGE_POSITION_UPDATED, 0, position_ptr);
		}
	}
}

void CTPTradeWorkerProcessor::UpdateSysYdPosition(const std::string& userId, const UserPositionExDO_Ptr& position_ptr)
{
	if (position_ptr->ExchangeID() != EXCHANGE_SHFE || position_ptr->PositionDateFlag == PositionDateFlagType::PSD_HISTORY)
	{
		SysPosition_HashMap posMap;
		if (!_ydSysPositions.find(userId, posMap))
		{
			_ydSysPositions.insert(userId, std::move(SysPosition_HashMap(4)));
			_ydSysPositions.find(userId, posMap);
		}

		posMap.map()->insert_or_assign(std::make_pair(position_ptr->InstrumentID(), position_ptr->Direction), position_ptr);
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

UserPositionExDO_Ptr CTPTradeWorkerProcessor::UpdatePosition(const TradeRecordDO_Ptr& trdDO_Ptr)
{
	UserPositionExDO_Ptr ret;

	if (!GetUserPositionContext()->ContainsTrade(trdDO_Ptr->TradeID128()))
	{
		int multiplier = 1;
		if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(trdDO_Ptr->InstrumentID()))
		{
			multiplier = pContractInfo->VolumeMultiple;
		}

		if (ret = GetUserPositionContext()->UpsertPosition(trdDO_Ptr->UserID(), trdDO_Ptr, multiplier, trdDO_Ptr->ExchangeID() != EXCHANGE_SHFE))
		{
			if (auto pMktDataMap = GetMarketDataDOMap())
			{
				MarketDataDO mdo;
				if (pMktDataMap->find(ret->InstrumentID(), mdo))
				{
					ret->LastPrice = mdo.LastPrice;
					ret->PreSettlementPrice = mdo.PreSettlementPrice;
				}
			}
		}

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

	SysPosition_HashMap posSysMap;
	if (_ydSysPositions.find(userId, posSysMap))
	{
		for (auto& pair : posSysMap.map()->lock_table())
		{
			bool found = false;
			if (!posDBMap.empty())
			{
				for (auto& dbpair : posDBMap.map()->lock_table())
				{
					std::string dbcontract, dbportfolio;
					PositionDirectionType dbdirection;
					std::tie(dbcontract, dbportfolio, dbdirection) = dbpair.first;

					if (dbcontract == pair.second->InstrumentID() && dbdirection == pair.second->Direction)
					{
						if (auto pPosition = positions.tryfind(dbpair.first))
						{
							pPosition->second = pair.second->YdInitPosition;
							found = true;
						}
					}
				}
			}

			if (!found)
			{
				positions.emplace(std::make_tuple(pair.first.first, "", pair.first.second), std::make_pair(0, pair.second->YdInitPosition));
			}
		}
	}

	return positions.size();
}