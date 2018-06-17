/***********************************************************************
 * Module:  XTTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Implementation of the class XTTradeWorkerProcessor
 ***********************************************************************/

#include "XTTradeWorkerProcessor.h"
#include "XTUtility.h"
#include "XTConstant.h"

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
 // Name:       XTTradeWorkerProcessor::XTTradeWorkerProcessor()
 // Purpose:    Implementation of XTTradeWorkerProcessor::XTTradeWorkerProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

XTTradeWorkerProcessor::XTTradeWorkerProcessor(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx)
	: CTPTradeWorkerProcessorBase(pServerCtx, positionCtx)
{
	if (_isQueryAccount)
		_accountQuery = std::async(std::launch::async, &XTTradeWorkerProcessor::QueryAccountWorker, this);
}

////////////////////////////////////////////////////////////////////////
// Name:       XTTradeWorkerProcessor::~XTTradeWorkerProcessor()
// Purpose:    Implementation of XTTradeWorkerProcessor::~XTTradeWorkerProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

XTTradeWorkerProcessor::~XTTradeWorkerProcessor()
{
	_closing = true;

	if (_accountQuery.valid())
		_accountQuery.wait();

	LOG_DEBUG << __FUNCTION__;
}

void XTTradeWorkerProcessor::Initialize(IServerContext * pServerCtx)
{
	Init(pServerCtx);
}

void XTTradeWorkerProcessor::QueryAccountWorker()
{
	while (!_closing)
	{
		std::set<std::string> userSet;
		_userSessionCtn_Ptr->forall([&userSet, this](const IMessageSession_Ptr& session_ptr)
		{
			if (auto proc = std::static_pointer_cast<XTTradeProcessor>(session_ptr->LockMessageProcessor()))
			{
				auto& userId = session_ptr->getUserInfo().getUserId();
				if (userSet.find(userId) == userSet.end())
				{
					userSet.emplace(userId);
					if (auto tdApi = proc->TradeApi())
						tdApi->get()->reqAccountDetail(session_ptr->getUserInfo().getInvestorId().data(), 0);
				}
			}
		});

		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

int XTTradeWorkerProcessor::RequestData(void)
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
	}

	return ret;
}

int XTTradeWorkerProcessor::LoginSystemUser(void)
{
	int ret = 0;

	TradeApi()->get()->userLogin(_systemUser.getBrokerId().data(), _systemUser.getPassword().data(), 0);

	return ret;
}

int XTTradeWorkerProcessor::LoginSystemUserIfNeed(void)
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
				CreateBackendAPI(this, _systemUser.getBrokerId(), exDO.Address);
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

int XTTradeWorkerProcessor::LogoutSystemUser(void)
{
	TradeApi()->get()->userLogout(_systemUser.getBrokerId().data(), _systemUser.getPassword().data(), 0);
	return 0;
}

int & XTTradeWorkerProcessor::GetDataLoadMask(void)
{
	return DataLoadMask;
}

TemplateMessageProcessor * XTTradeWorkerProcessor::GetTemplateProcessor(void)
{
	return this;
}

IServerContext * XTTradeWorkerProcessor::getServerContext(void)
{
	return XTTradeProcessor::getServerContext();
}


OrderDO_Ptr XTTradeWorkerProcessor::RefineOrder(const COrderDetail *pOrder)
{
	// update orders
	auto orderSysId = XTUtility::ToUInt64(pOrder->m_strOrderSysID);
	OrderDO_Ptr ctxOrder = orderSysId ? _userOrderCtx.FindOrder(orderSysId) : nullptr;
	auto orderptr = XTUtility::ParseRawOrder(pOrder, ctxOrder);

	if (orderptr->PortfolioID().empty())
	{
		OrderRequestDO orderReq;
		if (OrderReqCache::Find(OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID), orderReq))
		{
			orderptr->SetPortfolioID(orderReq.PortfolioID());
		}
		else if (orderSysId && OrderReqCache::Find(orderSysId, orderReq))
		{
			orderptr->SetPortfolioID(orderReq.PortfolioID());
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

TradeRecordDO_Ptr XTTradeWorkerProcessor::RefineTrade(const CDealDetail * pTrade)
{
	TradeRecordDO_Ptr trdDO_Ptr = XTUtility::ParseRawTrade(pTrade);

	if (trdDO_Ptr)
	{
		OrderRequestDO orderReq;
		if (OrderReqCache::Find(trdDO_Ptr->OrderSysID, orderReq))
		{
			trdDO_Ptr->SetPortfolioID(orderReq.PortfolioID());
		}
		else if (auto order_ptr = _userOrderCtx.FindOrder(trdDO_Ptr->OrderSysID))
		{
			if (!order_ptr->PortfolioID().empty())
			{
				trdDO_Ptr->SetPortfolioID(order_ptr->PortfolioID());
			}
			else if (OrderReqCache::Find(OrderSeqGen::GetOrderID(order_ptr->OrderID, order_ptr->SessionID), orderReq))
			{
				trdDO_Ptr->SetPortfolioID(orderReq.PortfolioID());
			}
		}
		else if (OrderReqCache::Find(OrderSeqGen::GetOrderID(trdDO_Ptr->OrderID, _systemUser.getSessionId()), orderReq))
		{
			trdDO_Ptr->SetPortfolioID(orderReq.PortfolioID());
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



///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void XTTradeWorkerProcessor::onConnected(bool success, const char* errorMsg)
{
	_isConnected = success;

	if (success)
	{
		auto loginTime = _systemUser.getLoginTime();
		auto now = std::time(nullptr);
		if (_isLogged && (now - loginTime) > 600)
		{
			LoginSystemUser();
		}

		LOG_INFO << getServerContext()->getServerUri() << ": Front Connected.";
	}
	else
	{
		LOG_WARN << getServerContext()->getServerUri() << " has disconnected: " << errorMsg;
	}

}

///登录请求响应
void XTTradeWorkerProcessor::onUserLogin(const char* userName, const char* password, int nRequestId, const XtError& error)
{
	if (XTUtility::HasError(&error))
	{
		LOG_ERROR << _serverCtx->getServerUri() << ": " << error.errorMsg();
	}
	else
	{
		std::string defaultUserId;
		if (SysParam::TryGet("DEFAULT.TRADINGDESK.ACCOUNTID", defaultUserId))
		{
			_systemUser.setUserId(defaultUserId);
		}

		_systemUser.setSessionId(getMessageSession()->Id());

		/*if (_loadPositionFromDB)
			LoadPositonFromDatabase(_systemUser.getUserId(), _systemUser.getInvestorId(),
				std::to_string(_systemUser.getTradingDay()));*/

		_isLogged = true;
	}
}

void XTTradeWorkerProcessor::onUserLogout(const char * userName, const char * password, int nRequestId, const XtError & error)
{
}

///报单录入错误回报
void XTTradeWorkerProcessor::onRtnOrderError(const COrderError* data)
{
	if (data)
	{
		if (auto orderptr = XTUtility::ParseRawOrder(data, _systemUser.getSessionId()))
		{
			OrderRequestDO orderReq;
			auto orderId = OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID);
			if (OrderReqCache::Find(orderId, orderReq))
			{
				orderptr->SetPortfolioID(orderReq.PortfolioID());
				OrderReqCache::Remove(orderId);
			}
			orderptr->HasMore = false;
			DispatchUserMessage(MSG_ID_ORDER_NEW, data->m_nRequestID, orderptr->UserID(), orderptr);
		}
	}
}

///报单操作错误回报
void XTTradeWorkerProcessor::onRtnCancelError(const CCancelError* data)
{
	if (auto orderptr = XTUtility::ParseRawOrder(pOrderAction, pRspInfo,
		_userOrderCtx.FindOrder(XTUtility::ToUInt64(pOrderAction->OrderSysID))))
	{
		orderptr->HasMore = false;
		DispatchUserMessage(MSG_ID_ORDER_CANCEL, pOrderAction->RequestID, orderptr->UserID(), orderptr);
	}
}

///报单录入请求响应
void XTTradeWorkerProcessor::onOrder(int nRequestId, int orderID, const XtError& error)
{
	if (auto orderptr = XTUtility::ParseRawOrder(pInputOrder, pRspInfo, _systemUser.getSessionId()))
	{
		if (orderptr->ErrorCode)
		{
			OrderRequestDO orderReq;
			auto orderId = OrderSeqGen::GetOrderID(orderptr->OrderID, orderptr->SessionID);
			if (OrderReqCache::Find(orderId, orderReq))
			{
				orderptr->SetPortfolioID(orderReq.PortfolioID());
				OrderReqCache::Remove(orderId);
			}
		}
		orderptr->HasMore = false;
		DispatchUserMessage(MSG_ID_ORDER_NEW, nRequestId, orderptr->UserID(), orderptr);
	}
}

void XTTradeWorkerProcessor::onCancelOrder(int nRequestId, const XtError& error)
{
	if (auto orderptr = XTUtility::ParseRawOrder(pInputOrderAction, pRspInfo,
		_userOrderCtx.FindOrder(XTUtility::ToUInt64(pInputOrderAction->OrderSysID))))
	{
		DispatchUserMessage(MSG_ID_ORDER_CANCEL, nRequestId, orderptr->UserID(), orderptr);
	}
}

void XTTradeWorkerProcessor::onRtnLoginStatus(const char * accountID, EBrokerLoginStatus status, int brokerType, const char * errorMsg)
{
	_systemUser.setUserId(accountID);

	std::string tradingDay;
	if (SysParam::TryGet(STR_KEY_APP_TRADINGDAY, tradingDay))
	{
		_systemUser.setTradingDay(std::stoi(tradingDay));
	}

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

	LOG_INFO << getServerContext()->getServerUri() << ": System user " << _systemUser.getInvestorId() << " has logged on trading server.";
	LOG_FLUSH;
}

void XTTradeWorkerProcessor::onRtnOrderDetail(const COrderDetail* data)
{
	if (data)
	{
		auto orderptr = RefineOrder(data);

		//if (auto msgId = XTUtility::ParseOrderMessageID(orderptr->OrderStatus))
		//{
		//	auto sid = msgId == MSG_ID_ORDER_CANCEL ? orderptr->OrderSysID : 0;
		//	DispatchUserMessage(msgId, sid, orderptr->UserID(), orderptr);
		//}

		DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
	}
}

void XTTradeWorkerProcessor::onRtnDealDetail(const CDealDetail* data)
{
	if (data)
	{
		_tradeCnt++;

		if (auto trdDO_Ptr = RefineTrade(data))
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
void XTTradeWorkerProcessor::onReqPositionDetail(const char* accountID, int nRequestId, const CPositionDetail* data, bool isLast, const XtError& error)
{
	std::string sysUserId = accountID;
	//sysUserId += pInvestorPosition->InvestorID;

	auto position = XTUtility::ParseRawPosition(data, sysUserId);
	UpdateSysYdPosition(sysUserId, position);

	if (!IsLoadPositionFromDB())
	{
		std::set<std::string> userSet;
		_userSessionCtn_Ptr->foreach(sysUserId, [data, &position, &sysUserId, &userSet, this](const IMessageSession_Ptr& session_ptr)
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

void XTTradeWorkerProcessor::onReqAccountDetail(const char* accountID, int nRequestId, const CAccountDetail* data, bool isLast, const XtError& error)
{
	AccountInfoDO_Ptr accountInfo_ptr;

	if (data)
	{
		accountInfo_ptr = XTUtility::ParseRawAccountInfo(data);

		std::string sysUserId = data->m_strAccountID;
		//sysUserId += data->AccountID;

		std::set<std::string> userSet;
		_userSessionCtn_Ptr->foreach(sysUserId, [&sysUserId, &accountInfo_ptr, &userSet, nRequestId, this](const IMessageSession_Ptr& session_ptr)
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

			SendDataObject(session_ptr, MSG_ID_QUERY_ACCOUNT_INFO, nRequestId, accountInfo_ptr);
		});
	}
}

