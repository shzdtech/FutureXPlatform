/***********************************************************************
 * Module:  CTPOTCTradeProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class CTPOTCTradeProcessor
 ***********************************************************************/

#include "CTPOTCTradeProcessor.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/CTPConstant.h"
#include "../CTPServer/CTPMapping.h"
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
#include "../ordermanager/OrderSeqGen.h"
#include "../ordermanager/OrderPortfolioCache.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"


CTPOTCTradeProcessor::CTPOTCTradeProcessor(
	IServerContext* pServerCtx,
	const IPricingDataContext_Ptr& pricingDataCtx)
	: OTCTradeProcessor(pricingDataCtx),
	CTPTradeWorkerSAProcessor(pServerCtx, pricingDataCtx),
	_otcOrderMgr(this, pricingDataCtx, this),
	_autoOrderMgr(this, pricingDataCtx, _userPositionCtx_Ptr),
	_hedgeOrderMgr(this, pricingDataCtx, _userPositionCtx_Ptr)
{
	_logTrades = false;

	_autoOrderWorker = std::async(std::launch::async, &CTPOTCTradeProcessor::AutoOrderWorker, this);
	_hedgeOrderWorker = std::async(std::launch::async, &CTPOTCTradeProcessor::HedgeOrderWorker, this);
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradeProcessor::~CTPOTCTradeProcessor()
{
	if (!_closing)
		Dispose();

	LOG_DEBUG << __FUNCTION__;
}

bool CTPOTCTradeProcessor::Dispose(void)
{
	_closing = true;

	if (_autoOrderWorker.valid())
		_autoOrderWorker.wait();

	if (_hedgeOrderWorker.valid())
		_hedgeOrderWorker.wait();


	return OTCTradeProcessor::Dispose();
}


void CTPOTCTradeProcessor::AutoOrderWorker()
{
	while (!_closing)
	{
		UserContractKey strategyKey;
		if (_autoOrderQueue.pop(strategyKey))
		{
			StrategyContractDO_Ptr strategy_ptr;
			if (PricingDataContext()->GetStrategyMap()->find(strategyKey, strategy_ptr))
			{
				_autoOrderMgr.TradeByStrategy(*strategy_ptr);
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void CTPOTCTradeProcessor::HedgeOrderWorker()
{
	while (!_closing)
	{
		PortfolioKey portfolioKey;
		if (_hedgeOrderQueue.pop(portfolioKey))
		{
			_hedgeOrderMgr.Hedge(portfolioKey);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void CTPOTCTradeProcessor::TriggerHedgeOrderUpdating(const PortfolioKey& portfolioKey)
{
	_hedgeOrderQueue.emplace(portfolioKey);
}

void CTPOTCTradeProcessor::TriggerAutoOrderUpdating(const StrategyContractDO& strategyDO)
{
	_autoOrderQueue.emplace(strategyDO);
}

void CTPOTCTradeProcessor::OnTraded(const TradeRecordDO_Ptr& tradeDO)
{
	DispatchUserMessage(MSG_ID_TRADE_RTN, 0, tradeDO->UserID(), tradeDO);

	if (auto position_ptr = GetOTCOrderManager().GetPositionContext().GetPosition(tradeDO->UserID(), tradeDO->PortfolioID(), *tradeDO))
	{
		DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, tradeDO->UserID(), position_ptr);
	}
}


OrderDO_Ptr CTPOTCTradeProcessor::CreateOrder(const OrderRequestDO& orderReq)
{
	// 端登成功,发出报单录入请求
	CThostFtdcInputOrderField req{};

	//经纪公司代码
	std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
	//投资者代码
	std::strncpy(req.InvestorID, _systemUser.getInvestorId().data(), sizeof(req.InvestorID));
	// 合约代码
	std::strncpy(req.InstrumentID, orderReq.InstrumentID().data(), sizeof(req.InstrumentID));
	///报单引用
	std::snprintf(req.OrderRef, sizeof(req.OrderRef), FMT_ORDERREF, orderReq.OrderID);
	// 用户代码
	std::strncpy(req.UserID, orderReq.UserID().data(), sizeof(req.UserID));
	// 报单价格条件
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	// 买卖方向
	req.Direction = orderReq.Direction > 0 ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open + orderReq.OpenClose;
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	// 价格
	req.LimitPrice = orderReq.LimitPrice;
	// 数量
	req.VolumeTotalOriginal = orderReq.Volume;
	// 有效期类型
	req.TimeCondition = orderReq.TIF == OrderTIFType::IOC ? THOST_FTDC_TC_IOC : THOST_FTDC_TC_GFD;
	// 成交量类型
	auto vit = CTPVolCondMapping.find((OrderVolType)orderReq.VolCondition);
	req.VolumeCondition = vit != CTPVolCondMapping.end() ? vit->second : THOST_FTDC_VC_AV;
	// 最小成交量
	req.MinVolume = 1;
	// 触发条件
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	// 止损价
	req.StopPrice = 0;
	// 强平原因
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	// 自动挂起标志
	req.IsAutoSuspend = false;

	OrderPortfolioCache::Insert(orderReq.OrderID, orderReq);

	auto tdApiProxy = _rawAPI->TdAPIProxy();
	if (!tdApiProxy || tdApiProxy->get()->ReqOrderInsert(&req, 0) != 0)
	{
		OrderPortfolioCache::Remove(orderReq.OrderID);
		return nullptr;
	}

	return CTPUtility::ParseRawOrder(&req, nullptr, orderReq.SessionID ? orderReq.SessionID : _systemUser.getSessionId());
}


OrderDO_Ptr CTPOTCTradeProcessor::CancelOrder(const OrderRequestDO& orderReq)
{
	CThostFtdcInputOrderActionField req{};

	req.ActionFlag = THOST_FTDC_AF_Delete;
	//经纪公司代码
	std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID));
	std::strncpy(req.InvestorID, _systemUser.getUserId().data(), sizeof(req.InvestorID));
	std::strncpy(req.UserID, orderReq.UserID().data(), sizeof(req.UserID));

	if (orderReq.OrderSysID != 0)
	{
		std::strncpy(req.ExchangeID, orderReq.ExchangeID().data(), sizeof(req.ExchangeID));
		std::snprintf(req.OrderSysID, sizeof(req.OrderSysID), FMT_ORDERSYSID, orderReq.OrderSysID);
	}
	else
	{
		req.FrontID = _systemUser.getFrontId();
		req.SessionID = orderReq.SessionID ? orderReq.SessionID : _systemUser.getSessionId();
		std::strncpy(req.InstrumentID, orderReq.InstrumentID().data(), sizeof(req.InstrumentID));
		std::snprintf(req.OrderRef, sizeof(req.OrderRef), FMT_ORDERREF, orderReq.OrderID);
	}

	auto tdApiProxy = _rawAPI->TdAPIProxy();
	if (!tdApiProxy || tdApiProxy->get()->ReqOrderAction(&req, AppContext::GenNextSeq()) != 0)
		return nullptr;

	req.SessionID = orderReq.SessionID ? orderReq.SessionID : _systemUser.getSessionId();

	return CTPUtility::ParseRawOrder(&req, nullptr);
}

uint32_t CTPOTCTradeProcessor::GetSessionId(void)
{
	return _systemUser.getSessionId();
}

OTCOrderManager& CTPOTCTradeProcessor::GetOTCOrderManager(void)
{
	return _otcOrderMgr;
}

AutoOrderManager& CTPOTCTradeProcessor::GetAutoOrderManager(void)
{
	return _autoOrderMgr;
}

HedgeOrderManager& CTPOTCTradeProcessor::GetHedgeOrderManager(void)
{
	return _hedgeOrderMgr;
}

UserOrderContext& CTPOTCTradeProcessor::GetExchangeOrderContext()
{
	return _userOrderCtx;
}


//CTP API

///登录请求响应
void CTPOTCTradeProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CTPTradeWorkerSAProcessor::OnRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);
	if (!CTPUtility::HasError(pRspInfo))
	{
		_autoOrderMgr.Reset();
	}
}

///报单录入请求响应
void CTPOTCTradeProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, _systemUser.getSessionId());
		_autoOrderMgr.OnMarketOrderUpdated(*orderptr);
		_hedgeOrderMgr.OnMarketOrderUpdated(*orderptr);

		if (CTPUtility::HasError(pRspInfo))
		{
			StrategyContractDO_Ptr strategy_ptr;
			_pricingCtx->GetStrategyMap()->find(*orderptr, strategy_ptr);
			if (strategy_ptr)
			{
				strategy_ptr->AutoOrderSettings.LimitOrderCounter--;
				DispatchUserMessage(MSG_ID_MODIFY_STRATEGY, 0, strategy_ptr->UserID(), strategy_ptr);
			}

			DispatchUserMessage(MSG_ID_ORDER_NEW, 0, orderptr->UserID(), orderptr);
		}
	}
}

void CTPOTCTradeProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		if (pInputOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			auto orderptr = CTPUtility::ParseRawOrder(pInputOrderAction, pRspInfo);
			_autoOrderMgr.OnMarketOrderUpdated(*orderptr);
			_hedgeOrderMgr.OnMarketOrderUpdated(*orderptr);
		}
	}
}

///报单录入错误回报
void CTPOTCTradeProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if (pInputOrder)
	{
		if (auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, _systemUser.getSessionId()))
		{
			_autoOrderMgr.OnMarketOrderUpdated(*orderptr);
			_hedgeOrderMgr.OnMarketOrderUpdated(*orderptr);

			StrategyContractDO_Ptr strategy_ptr;
			_pricingCtx->GetStrategyMap()->find(*orderptr, strategy_ptr);
			if (strategy_ptr)
			{
				strategy_ptr->AutoOrderSettings.LimitOrderCounter--;
				DispatchUserMessage(MSG_ID_MODIFY_STRATEGY, 0, strategy_ptr->UserID(), strategy_ptr);
			}


			DispatchUserMessage(MSG_ID_ORDER_NEW, 0, orderptr->UserID(), orderptr);
		}
	}
}

///报单操作错误回报
void CTPOTCTradeProcessor::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	if (pOrderAction)
	{
		if (pOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			if (auto orderptr = CTPUtility::ParseRawOrder(pOrderAction, pRspInfo))
			{
				_autoOrderMgr.OnMarketOrderUpdated(*orderptr);
				_hedgeOrderMgr.OnMarketOrderUpdated(*orderptr);
			}
		}
	}
}

void CTPOTCTradeProcessor::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder)
	{
		bool sendNotification = false;

		if (pOrder->TimeCondition != THOST_FTDC_TC_IOC)
		{
			if (int orderSysId = CTPUtility::ToUInt64(pOrder->OrderSysID))
			{
				sendNotification = !_userOrderCtx.FindOrder(orderSysId);
			}
		}

		auto orderptr = RefineOrder(pOrder);

		StrategyContractDO_Ptr strategy_ptr;

		if (sendNotification)
		{
			int limitOrders = _userOrderCtx.GetLimitOrderCount(orderptr->InstrumentID());

			if (_pricingCtx->GetStrategyMap()->find(*orderptr, strategy_ptr))
			{
				if (strategy_ptr->AutoOrderSettings.LimitOrderCounter < limitOrders)
				{
					strategy_ptr->AutoOrderSettings.LimitOrderCounter = limitOrders;
				}
			}
		}

		// update auto orders
		int ret = _autoOrderMgr.OnMarketOrderUpdated(*orderptr);
		if (ret != 0)
		{
			if (!strategy_ptr)
				_pricingCtx->GetStrategyMap()->find(*orderptr, strategy_ptr);

			if (strategy_ptr)
			{
				if (ret > 0)
				{
					sendNotification = true;

					if (orderptr->Direction == DirectionType::SELL)
					{
						strategy_ptr->AutoOrderSettings.AskCounter += ret;
					}
					else
					{
						strategy_ptr->AutoOrderSettings.BidCounter += ret;
					}
				}
				else if (ret < 0 && !orderptr->OrderSysID)
				{
					strategy_ptr->AutoOrderSettings.LimitOrderCounter--;
				}
			}

			if (ret >= -1)
				_autoOrderQueue.emplace(*orderptr);
		}

		// update hedge orders
		ret = _hedgeOrderMgr.OnMarketOrderUpdated(*orderptr);
		if (!orderptr->PortfolioID().empty())
		{
			auto orderStatus = orderptr->OrderStatus;
			if (orderStatus == OrderStatusType::PARTIAL_TRADING ||
				orderStatus == OrderStatusType::ALL_TRADED ||
				orderStatus == OrderStatusType::CANCELED)
			{
				_hedgeOrderQueue.emplace(*orderptr);
			}
		}

		if (sendNotification)
		{
			if (!strategy_ptr)
				_pricingCtx->GetStrategyMap()->find(*orderptr, strategy_ptr);

			if (strategy_ptr)
				DispatchUserMessage(MSG_ID_MODIFY_STRATEGY, 0, strategy_ptr->UserID(), strategy_ptr);
		}
	}
}

void CTPOTCTradeProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	auto& userInfo = sessionPtr->getUserInfo();
	userInfo.setTradingDay(_systemUser.getTradingDay());
	userInfo.setFrontId(_systemUser.getFrontId());
	userInfo.setSessionId(_systemUser.getSessionId());
	_userSessionCtn_Ptr->add(userInfo.getUserId(), sessionPtr);
}

void CTPOTCTradeProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		if (auto trdDO_Ptr = RefineTrade(pTrade))
		{
			if (_logTrades)
				_tradeQueue.push(trdDO_Ptr);

			UpdatePosition(trdDO_Ptr);
		}
	}
}