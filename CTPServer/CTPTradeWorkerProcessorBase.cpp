/***********************************************************************
 * Module:  CTPTradeWorkerProcessorBase.cpp
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Implementation of the class CTPTradeWorkerProcessorBase
 ***********************************************************************/

#include "CTPTradeWorkerProcessorBase.h"
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
#include "../ordermanager/OrderReqCache.h"
#include "../bizutility/ModelParamsCache.h"
#include "../riskmanager/RiskModelAlgorithmManager.h"
#include "../riskmanager/RiskContext.h"
#include "../riskmanager/RiskModelParams.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeWorkerProcessorBase::CTPTradeWorkerProcessorBase()
 // Purpose:    Implementation of CTPTradeWorkerProcessorBase::CTPTradeWorkerProcessorBase()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessorBase::CTPTradeWorkerProcessorBase(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx)
	: _accountInfoMap(4), _ydDBPositions(4), _ydSysPositions(4), _logTrades(false), _loadPositionFromDB(false), _pMktDataMap(nullptr),
	_sharedSystemSessionHub(SessionContainer<std::string>::NewInstancePtr())
{
	_userPositionCtx_Ptr = positionCtx ? positionCtx : std::make_shared<PortfolioPositionContext>();

	std::string defaultCfg;
	pServerCtx->getConfigVal(ExchangeRouterTable::TARGET_TD, defaultCfg);
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
	if (pServerCtx->getConfigVal(SAVE_TRADE, saveTrade))
	{
		_logTrades = stringutility::compare(saveTrade, "true") == 0;
	}

	if (_logTrades)
		_tradeDBSerializer = std::async(std::launch::async, &CTPTradeWorkerProcessorBase::LogTrade, this);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessorBase::~CTPTradeWorkerProcessorBase()
// Purpose:    Implementation of CTPTradeWorkerProcessorBase::~CTPTradeWorkerProcessorBase()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPTradeWorkerProcessorBase::~CTPTradeWorkerProcessorBase()
{
	_closing = true;

	if (_initializer.valid())
		_initializer.wait();

	if (_tradeDBSerializer.valid())
		_tradeDBSerializer.wait();

	LOG_DEBUG << __FUNCTION__;
}

void CTPTradeWorkerProcessorBase::LogTrade()
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


bool CTPTradeWorkerProcessorBase::IsLoadPositionFromDB()
{
	return _loadPositionFromDB;
}

bool CTPTradeWorkerProcessorBase::IsSaveTrade()
{
	return _logTrades;
}

void CTPTradeWorkerProcessorBase::OnNewManualTrade(const TradeRecordDO & tradeDO)
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

void CTPTradeWorkerProcessorBase::OnUpdateManualPosition(const UserPositionExDO & positionDO)
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

void CTPTradeWorkerProcessorBase::OnUpdateMarketData(const MarketDataDO & mdDO)
{
	std::set<std::string> userSet;
	_userSessionCtn_Ptr->forall([this, &userSet, &mdDO](const IMessageSession_Ptr& msgSession)
	{
		auto& userId = msgSession->getUserInfo().getUserId();
		if (userSet.find(userId) == userSet.end())
		{
			userSet.emplace(userId);
			GetUserPositionContext()->UpdatePnL(userId, mdDO);
		}
	});
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeWorkerProcessorBase::OnInit()
// Purpose:    Implementation of CTPTradeWorkerProcessorBase::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPTradeWorkerProcessorBase::Init(IServerContext* pServerCtx)
{
	std::string productTypes;
	if (pServerCtx->getConfigVal("product_type", productTypes))
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

void CTPTradeWorkerProcessorBase::LoginUserSession(const CTPProcessor_Ptr & sessionPtr, const std::string & brokerId, const std::string & investorId, const std::string & password, const std::string & serverName)
{
}

bool CTPTradeWorkerProcessorBase::IsUserLogged(const std::string & userId)
{
	return false;
}

int CTPTradeWorkerProcessorBase::LoadContractFromDB()
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

	GetDataLoadMask() |= DataLoadType::INSTRUMENT_DATA_LOADED;

	return 0;
}

int CTPTradeWorkerProcessorBase::LoadDataAsync(void)
{
	std::string load_contract_db;
	if (getServerContext()->getConfigVal("load_contract_db", load_contract_db) &&
		stringutility::compare(load_contract_db.data(), "true") == 0)
	{
		LoadContractFromDB();
	}

	bool loaded = GetDataLoadMask() & (INSTRUMENT_DATA_LOADED);

	if (!loaded)
	{
		_initializer = std::async(std::launch::async,
			[this]() {
			auto millsec = std::chrono::milliseconds(500);
			bool loaded = GetDataLoadMask() & (INSTRUMENT_DATA_LOADED);
			int i = 0;
			while (!_closing && !loaded && i++ < RetryTimes)
			{
				if (!LoginSystemUserIfNeed())
				{
					std::this_thread::sleep_for(std::chrono::seconds(3)); //wait 3 secs for login CTP server
					RequestData();
				}

				for (int cum_ms = 0; cum_ms < RetryInterval && !_closing && !loaded; cum_ms += millsec.count())
				{
					std::this_thread::sleep_for(millsec);
					loaded = GetDataLoadMask() & (INSTRUMENT_DATA_LOADED);
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

void CTPTradeWorkerProcessorBase::OnDataLoaded(void)
{
	LOG_INFO << getServerContext()->getServerUri() << ": Data preloading finished.";

	std::string logout_dataloaded;
	if (getServerContext()->getConfigVal("logout_dataloaded", logout_dataloaded) &&
		stringutility::compare(logout_dataloaded.data(), "true") == 0)
	{
		LogoutSystemUser();
		_isLogged = false;

		LOG_INFO << getServerContext()->getServerUri() << ": System user has logout.";
	}

	LOG_FLUSH;
}

void CTPTradeWorkerProcessorBase::LoadPositonFromDatabase(const std::string & userId, const std::string& tradingday)
{
	try
	{
		auto mktDataMap = std::static_pointer_cast<MarketDataDOMap>(AppContext::GetData(STR_KEY_APP_MARKETDATA));
		if (auto positionVec_ptr = PositionDAO::QueryLastDayPosition(userId, tradingday))
		{
			for (auto& position : *positionVec_ptr)
			{
				if (mktDataMap)
				{
					MarketDataDO mdo;
					if (mktDataMap->find(position.InstrumentID(), mdo))
					{
						position.LastPrice = mdo.LastPrice;
						position.PreSettlementPrice = mdo.PreSettlementPrice;
					}

					if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(position.InstrumentID()))
					{
						position.Multiplier = pContractInfo->VolumeMultiple;
					}
				}
				Position_HashMap posMap;
				if (!_ydDBPositions.find(userId, posMap))
				{
					_ydDBPositions.insert(userId, std::move(Position_HashMap(4)));
					_ydDBPositions.find(userId, posMap);
				}
				posMap.map()->insert_or_assign(std::make_tuple(position.InstrumentID(), position.PortfolioID(), position.Direction), std::make_shared<UserPositionExDO>(position));
				GetUserPositionContext()->UpsertPosition(userId, position, true, position.ExchangeID() != EXCHANGE_SHFE);
			}
		}

		if (auto trades = TradeDAO::QueryExchangeTrade(userId, "", tradingday, tradingday))
		{
			for (auto& trade : *trades)
			{
				if (!trade.PortfolioID().empty())
				{
					OrderReqCache::Insert(trade.OrderSysID, OrderRequestDO(trade, trade.PortfolioID()));
				}

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

UserPositionExDO_Ptr CTPTradeWorkerProcessorBase::FindDBYdPostion(const std::string& userid,
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

UserPositionExDO_Ptr CTPTradeWorkerProcessorBase::FindSysYdPostion(const std::string& userid,
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

MarketDataDOMap * CTPTradeWorkerProcessorBase::GetMarketDataDOMap()
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

UserOrderContext& CTPTradeWorkerProcessorBase::GetExchangeOrderContext()
{
	return _userOrderCtx;
}

void CTPTradeWorkerProcessorBase::PushToLogQueue(const TradeRecordDO_Ptr & tradeDO_Ptr)
{
	if (_logTrades)
		_tradeQueue.push(tradeDO_Ptr);
}

void CTPTradeWorkerProcessorBase::UpdateSysYdPosition(const std::string& userId, const UserPositionExDO_Ptr& position_ptr)
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

UserPositionExDO_Ptr CTPTradeWorkerProcessorBase::UpdatePosition(const TradeRecordDO_Ptr& trdDO_Ptr)
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


void CTPTradeWorkerProcessorBase::DispatchUserMessage(int msgId, int serialId, const std::string& userId,
	const dataobj_ptr& dataobj_ptr)
{
	if (auto msg = GetTemplateProcessor()->Deserialize(msgId, serialId, dataobj_ptr))
	{
		_userSessionCtn_Ptr->foreach(userId, [msgId, &msg, this](const IMessageSession_Ptr& session_ptr)
		{
			GetTemplateProcessor()->SendDataObject(session_ptr, msgId, msg); 
		});
	}
}

void CTPTradeWorkerProcessorBase::DispatchMessageForAll(int msgId, int serialId, const dataobj_ptr& dataobj_ptr)
{
	if (auto msg = GetTemplateProcessor()->Deserialize(msgId, serialId, dataobj_ptr))
	{
		_userSessionCtn_Ptr->forall([msgId, &msg, this](const IMessageSession_Ptr& session_ptr)
		{
			GetTemplateProcessor()->SendDataObject(session_ptr, msgId, msg);
		});
	}
}


AccountInfoDO_Ptr CTPTradeWorkerProcessorBase::GetAccountInfo(const std::string& userId)
{
	AccountInfoDO_Ptr ret;
	_accountInfoMap.find(userId, ret);
	return ret;
}

void CTPTradeWorkerProcessorBase::UpdateAccountInfo(const std::string& userId, const AccountInfoDO & accoutInfo)
{
	auto account_ptr = std::make_shared<AccountInfoDO>(accoutInfo);
	_accountInfoMap.upsert(userId, [&account_ptr](AccountInfoDO_Ptr & accoutInfo_ptr) {accoutInfo_ptr = account_ptr; }, account_ptr);
}

std::set<ExchangeDO>& CTPTradeWorkerProcessorBase::GetExchangeInfo()
{
	return _exchangeInfo_Set;
}

IUserPositionContext_Ptr& CTPTradeWorkerProcessorBase::GetUserPositionContext()
{
	return _userPositionCtx_Ptr;
}

UserTradeContext& CTPTradeWorkerProcessorBase::GetUserTradeContext()
{
	return _userTradeCtx;
}

UserOrderContext & CTPTradeWorkerProcessorBase::GetUserOrderContext(void)
{
	return _userOrderCtx;
}

std::set<ProductType>& CTPTradeWorkerProcessorBase::GetProductTypeToLoad()
{
	return _productTypes;
}

int CTPTradeWorkerProcessorBase::ComparePosition(const std::string& userId, const std::string& sysUserId,
	autofillmap<std::tuple<std::string, std::string, PositionDirectionType>, std::pair<int, int>>& positions)
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
	if (_ydSysPositions.find(sysUserId, posSysMap))
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