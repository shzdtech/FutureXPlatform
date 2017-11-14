/***********************************************************************
 * Module:  CTPOTCTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class CTPOTCTradeWorkerProcessor
 ***********************************************************************/

#include "CTPOTCTradeWorkerProcessor.h"
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


CTPOTCTradeWorkerProcessor::CTPOTCTradeWorkerProcessor(
	IServerContext* pServerCtx,
	const IPricingDataContext_Ptr& pricingDataCtx,
	const IUserPositionContext_Ptr& positionCtx)
	: OTCTradeWorkerProcessor(pricingDataCtx),
	CTPTradeWorkerSAProcessor(pServerCtx, pricingDataCtx, positionCtx),
	_otcOrderMgr(pricingDataCtx, this),
	_autoOrderMgr(pricingDataCtx, _userPositionCtx_Ptr),
	_hedgeOrderMgr(pricingDataCtx, _userPositionCtx_Ptr)
{
	_logTrades = false;

	_autoOrderWorker = std::async(std::launch::async, &CTPOTCTradeWorkerProcessor::AutoOrderWorker, this);
	_hedgeOrderWorker = std::async(std::launch::async, &CTPOTCTradeWorkerProcessor::HedgeOrderWorker, this);
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradeWorkerProcessor::~CTPOTCTradeWorkerProcessor()
{
	if (!_closing)
		Dispose();

	LOG_DEBUG << __FUNCTION__;
}

bool CTPOTCTradeWorkerProcessor::Dispose(void)
{
	_closing = true;

	if (_autoOrderWorker.valid())
		_autoOrderWorker.wait();

	if (_hedgeOrderWorker.valid())
		_hedgeOrderWorker.wait();


	return OTCTradeProcessor::Dispose();
}


void CTPOTCTradeWorkerProcessor::AutoOrderWorker()
{
	while (!_closing)
	{
		std::pair<UserContractKey, OTCTradeProcessor_Ptr> strategyKey;
		if (_autoOrderQueue.pop(strategyKey) && strategyKey.second)
		{
			StrategyContractDO_Ptr strategy_ptr;
			if (PricingDataContext()->GetStrategyMap()->find(strategyKey.first, strategy_ptr))
			{
				_autoOrderMgr.TradeByStrategy(*strategy_ptr, strategyKey.second.get());
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void CTPOTCTradeWorkerProcessor::HedgeOrderWorker()
{
	while (!_closing)
	{
		std::pair<PortfolioKey, OTCTradeProcessor_Ptr> portfolioKey;
		if (_hedgeOrderQueue.pop(portfolioKey) && portfolioKey.second)
		{
			auto status = _hedgeOrderMgr.Hedge(portfolioKey.first, portfolioKey.second.get());
			if (status == HedgeOrderManager::Waiting)
			{
				_waitingHedgeQueue.emplace(portfolioKey);
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			while (_waitingHedgeQueue.pop(portfolioKey))
			{
				_hedgeOrderQueue.emplace(portfolioKey);
			}
		}
	}
}

void CTPOTCTradeWorkerProcessor::OnRtnOrder(const OTCTradeProcessor_Ptr & tradeProcessor, CThostFtdcOrderField * pOrder)
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
		int ret = _autoOrderMgr.OnMarketOrderUpdated(*orderptr, this);
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
				else if (ret < 0 && orderptr->TIF != OrderTIFType::IOC && !orderptr->OrderSysID)
				{
					strategy_ptr->AutoOrderSettings.LimitOrderCounter--;
				}
			}

			if (ret >= -1 && tradeProcessor)
				_autoOrderQueue.emplace(std::make_pair(*orderptr, tradeProcessor));
		}

		// update hedge orders
		ret = _hedgeOrderMgr.OnMarketOrderUpdated(*orderptr, this);
		if (!orderptr->PortfolioID().empty())
		{
			auto orderStatus = orderptr->OrderStatus;
			if (orderStatus == OrderStatusType::PARTIAL_TRADING ||
				orderStatus == OrderStatusType::ALL_TRADED ||
				orderStatus == OrderStatusType::CANCELED)
			{
				TriggerHedgeOrderUpdating(*orderptr);
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

void CTPOTCTradeWorkerProcessor::TriggerHedgeOrderUpdating(const PortfolioKey& portfolioKey)
{
	_userSessionCtn_Ptr->foreach(portfolioKey.UserID(), [this, &portfolioKey](const IMessageSession_Ptr& session_ptr)
	{
		if(auto processor_ptr = session_ptr->LockMessageProcessor())
			_hedgeOrderQueue.emplace(std::make_pair(portfolioKey, std::static_pointer_cast<CTPOTCTradeProcessor>(processor_ptr)));
	});
}

void CTPOTCTradeWorkerProcessor::TriggerAutoOrderUpdating(const StrategyContractDO& strategyDO)
{
	_userSessionCtn_Ptr->foreach(strategyDO.UserID(), [this, &strategyDO](const IMessageSession_Ptr& session_ptr)
	{
		if (auto processor_ptr = session_ptr->LockMessageProcessor())
			_autoOrderQueue.emplace(std::make_pair(strategyDO, std::static_pointer_cast<CTPOTCTradeProcessor>(processor_ptr)));
	});
}

void CTPOTCTradeWorkerProcessor::OnTraded(const TradeRecordDO_Ptr& tradeDO)
{
	DispatchUserMessage(MSG_ID_TRADE_RTN, 0, tradeDO->UserID(), tradeDO);

	if (auto position_ptr = GetOTCOrderManager().GetPositionContext().GetPosition(tradeDO->UserID(), tradeDO->PortfolioID(), *tradeDO))
	{
		DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, tradeDO->UserID(), position_ptr);
	}
}


OrderDO_Ptr CTPOTCTradeWorkerProcessor::CreateOrder(const OrderRequestDO& orderReq)
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


OrderDO_Ptr CTPOTCTradeWorkerProcessor::CancelOrder(const OrderRequestDO& orderReq)
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

uint32_t CTPOTCTradeWorkerProcessor::GetSessionId(void)
{
	return _systemUser.getSessionId();
}

OTCOrderManager& CTPOTCTradeWorkerProcessor::GetOTCOrderManager(void)
{
	return _otcOrderMgr;
}

AutoOrderManager& CTPOTCTradeWorkerProcessor::GetAutoOrderManager(void)
{
	return _autoOrderMgr;
}

HedgeOrderManager& CTPOTCTradeWorkerProcessor::GetHedgeOrderManager(void)
{
	return _hedgeOrderMgr;
}

UserOrderContext& CTPOTCTradeWorkerProcessor::GetExchangeOrderContext()
{
	return _userOrderCtx;
}

void CTPOTCTradeWorkerProcessor::OnNewManualTrade(const TradeRecordDO & tradeDO)
{
	GetUserTradeContext().InsertTrade(tradeDO);

	auto tradeDO_Ptr = std::make_shared<TradeRecordDO>(tradeDO);

	int multiplier = 1;
	if (auto pContractInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(tradeDO.InstrumentID()))
	{
		multiplier = pContractInfo->VolumeMultiple;
	}

	GetUserPositionContext()->UpsertPosition(tradeDO.UserID(), tradeDO_Ptr, multiplier);
}


//CTP API

///登录请求响应
void CTPOTCTradeWorkerProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CTPTradeWorkerSAProcessor::OnRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);
	if (!CTPUtility::HasError(pRspInfo))
	{
		_autoOrderMgr.CancelUserOrders(_systemUser.getUserId(), this);
	}
}

///报单录入请求响应
void CTPOTCTradeWorkerProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, _systemUser.getSessionId());
		_autoOrderMgr.OnMarketOrderUpdated(*orderptr, this);
		_hedgeOrderMgr.OnMarketOrderUpdated(*orderptr, this);

		if (CTPUtility::HasError(pRspInfo))
		{
			StrategyContractDO_Ptr strategy_ptr;
			_pricingCtx->GetStrategyMap()->find(*orderptr, strategy_ptr);
			if (strategy_ptr && orderptr->TIF != OrderTIFType::IOC)
			{
				strategy_ptr->AutoOrderSettings.LimitOrderCounter--;
				strategy_ptr->BidEnabled = false;
				strategy_ptr->AskEnabled = false;
				strategy_ptr->Hedging = false;
				DispatchUserMessage(MSG_ID_MODIFY_STRATEGY, 0, strategy_ptr->UserID(), strategy_ptr);
			}

			DispatchUserMessage(MSG_ID_ORDER_NEW, 0, orderptr->UserID(), orderptr);
		}
	}
}

void CTPOTCTradeWorkerProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		if (pInputOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			auto orderptr = CTPUtility::ParseRawOrder(pInputOrderAction, pRspInfo);
			_autoOrderMgr.OnMarketOrderUpdated(*orderptr, this);
			_hedgeOrderMgr.OnMarketOrderUpdated(*orderptr, this);
		}
	}
}

///报单录入错误回报
void CTPOTCTradeWorkerProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if (pInputOrder)
	{
		if (auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, _systemUser.getSessionId()))
		{
			_autoOrderMgr.OnMarketOrderUpdated(*orderptr, this);
			_hedgeOrderMgr.OnMarketOrderUpdated(*orderptr, this);

			StrategyContractDO_Ptr strategy_ptr;
			_pricingCtx->GetStrategyMap()->find(*orderptr, strategy_ptr);
			if (strategy_ptr && orderptr->TIF != OrderTIFType::IOC)
			{
				strategy_ptr->AutoOrderSettings.LimitOrderCounter--;
				strategy_ptr->BidEnabled = false;
				strategy_ptr->AskEnabled = false;
				strategy_ptr->Hedging = false;
				DispatchUserMessage(MSG_ID_MODIFY_STRATEGY, 0, strategy_ptr->UserID(), strategy_ptr);
			}


			DispatchUserMessage(MSG_ID_ORDER_NEW, 0, orderptr->UserID(), orderptr);
		}
	}
}

///报单操作错误回报
void CTPOTCTradeWorkerProcessor::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	if (pOrderAction)
	{
		if (pOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			if (auto orderptr = CTPUtility::ParseRawOrder(pOrderAction, pRspInfo))
			{
				_autoOrderMgr.OnMarketOrderUpdated(*orderptr, this);
				_hedgeOrderMgr.OnMarketOrderUpdated(*orderptr, this);
			}
		}
	}
}

void CTPOTCTradeWorkerProcessor::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	OnRtnOrder(std::static_pointer_cast<CTPOTCTradeWorkerProcessor>(shared_from_this()), pOrder);
}

void CTPOTCTradeWorkerProcessor::RegisterLoggedSession(const IMessageSession_Ptr& sessionPtr)
{
	auto& userInfo = sessionPtr->getUserInfo();
	if (!userInfo.getSessionId())
	{
		userInfo.setTradingDay(_systemUser.getTradingDay());
		userInfo.setFrontId(_systemUser.getFrontId());
		userInfo.setSessionId(_systemUser.getSessionId());
	}

	_userSessionCtn_Ptr->add(userInfo.getUserId(), sessionPtr);
}

void CTPOTCTradeWorkerProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		if (auto trdDO_Ptr = RefineTrade(pTrade))
		{
			PushToLogQueue(trdDO_Ptr);
			UpdatePosition(trdDO_Ptr);
		}
	}
}

void CTPOTCTradeWorkerProcessor::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast)
		DataLoadMask |= CTPProcessor::POSITION_DATA_LOADED;
}