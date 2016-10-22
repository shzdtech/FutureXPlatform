/***********************************************************************
 * Module:  CTPOTCTradeProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class CTPOTCTradeProcessor
 ***********************************************************************/

#include "CTPOTCTradeProcessor.h"
#include "../CTPServer/CTPConstant.h"
#include "../CTPServer/CTPUtility.h"

#include "../common/Attribute_Key.h"

#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../systemsettings/AppContext.h"
#include "../databaseop/TradeDAO.h"
#include "../litelogger/LiteLogger.h"


CTPOTCTradeProcessor::CTPOTCTradeProcessor(
	IServerContext* pServerCtx,
	const IPricingDataContext_Ptr& pricingDataCtx)
	: OTCTradeProcessor(pricingDataCtx), CTPTradeWorkerProcessor(pServerCtx),
	_otcOrderMgr(this, pricingDataCtx),
	_autoOrderMgr(this, pricingDataCtx)
{
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradeProcessor::~CTPOTCTradeProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

OrderDOVec_Ptr CTPOTCTradeProcessor::TriggerHedgeOrderUpdating(const StrategyContractDO& strategyDO)
{
	if (auto updatedOrders = OTCTradeProcessor::TriggerHedgeOrderUpdating(strategyDO))
	{
		for (auto& order : *updatedOrders)
		{
			auto orderptr = std::make_shared<OrderDO>(order);
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
		}
		return updatedOrders;
	}

	return nullptr;
}

OrderDOVec_Ptr CTPOTCTradeProcessor::TriggerOTCOrderUpdating(const StrategyContractDO& strategyDO)
{
	if (auto updatedOrders = OTCTradeProcessor::TriggerOTCOrderUpdating(strategyDO))
	{
		for (auto& order : *updatedOrders)
		{
			auto orderptr = std::make_shared<OrderDO>(order);
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
		}
		return updatedOrders;
	}
	return nullptr;
}



OrderDO_Ptr CTPOTCTradeProcessor::CreateOrder(OrderRequestDO& orderInfo)
{
	// 端登成功,发出报单录入请求
	CThostFtdcInputOrderField req{};

	//经纪公司代码
	std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID) - 1);
	//投资者代码
	std::strncpy(req.InvestorID, _systemUser.getInvestorId().data(), sizeof(req.InvestorID) - 1);
	// 合约代码
	std::strncpy(req.InstrumentID, orderInfo.InstrumentID().data(), sizeof(req.InstrumentID) - 1);
	///报单引用
	std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, orderInfo.OrderID);
	// 用户代码
	std::strncpy(req.UserID, orderInfo.UserID().data(), sizeof(req.UserID) - 1);
	// 报单价格条件
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	// 买卖方向
	req.Direction = orderInfo.Direction > 0 ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open + orderInfo.OpenClose;
	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	// 价格
	req.LimitPrice = orderInfo.LimitPrice;
	// 数量
	req.VolumeTotalOriginal = orderInfo.Volume;
	// 有效期类型
	req.TimeCondition = orderInfo.TIF == OrderTIFType::IOC ? THOST_FTDC_TC_IOC : THOST_FTDC_TC_GFD;
	// 成交量类型
	req.VolumeCondition = THOST_FTDC_VC_AV;
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

	if(_rawAPI->TrdAPI->ReqOrderInsert(&req, 0) != 0)
		return nullptr;

	return CTPUtility::ParseRawOrder(&req, nullptr, _systemUser.getSessionId());
}


OrderDO_Ptr CTPOTCTradeProcessor::CancelOrder(OrderRequestDO& orderInfo)
{
	CThostFtdcInputOrderActionField req{};

	req.ActionFlag = THOST_FTDC_AF_Delete;
	//经纪公司代码
	std::strncpy(req.BrokerID, _systemUser.getBrokerId().data(), sizeof(req.BrokerID) - 1);
	std::strncpy(req.InvestorID, _systemUser.getUserId().data(), sizeof(req.InvestorID) - 1);
	std::strncpy(req.UserID, orderInfo.UserID().data(), sizeof(req.UserID) - 1);
	if (orderInfo.OrderSysID != 0)
	{
		std::strncpy(req.ExchangeID, orderInfo.ExchangeID().data(), sizeof(req.ExchangeID) - 1);
		std::sprintf(req.OrderSysID, FMT_PADDING_ORDERSYSID, orderInfo.OrderSysID);
	}
	else
	{
		req.SessionID = _systemUser.getSessionId();
		req.FrontID = _systemUser.getFrontId();
		std::strncpy(req.InstrumentID, orderInfo.InstrumentID().data(), sizeof(req.InstrumentID) - 1);
		std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, orderInfo.OrderID);
	}

	if (_rawAPI->TrdAPI->ReqOrderAction(&req, AppContext::GenNextSeq()) != 0)
		return nullptr;
	
	req.SessionID = _systemUser.getSessionId();
	return CTPUtility::ParseRawOrder(&req, nullptr);
}

OTCOrderManager * CTPOTCTradeProcessor::GetOTCOrderManager(void)
{
	return &_otcOrderMgr;
}

AutoOrderManager * CTPOTCTradeProcessor::GetAutoOrderManager(void)
{
	return &_autoOrderMgr;
}


//CTP API

///登录请求响应
void CTPOTCTradeProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspUserLogin && !CTPUtility::HasError(pRspInfo))
	{
		if (auto sessionptr = LockMessageSession())
		{
			auto pUser = sessionptr->getUserInfo();
			pUser->setSessionId(pRspUserLogin->SessionID);
			pUser->setFrontId(pRspUserLogin->FrontID);
			_systemUser.setSessionId(pRspUserLogin->SessionID);
			_systemUser.setFrontId(pRspUserLogin->FrontID);

			_autoOrderMgr.Reset();
		}
	}

	CThostFtdcSettlementInfoConfirmField reqsettle{};
	std::strncpy(reqsettle.BrokerID, _systemUser.getBrokerId().data(), sizeof(reqsettle.BrokerID) - 1);
	std::strncpy(reqsettle.InvestorID, _systemUser.getInvestorId().data(), sizeof(reqsettle.InvestorID) - 1);
	_rawAPI->TrdAPI->ReqSettlementInfoConfirm(&reqsettle, 0);

	_isLogged = true;
	LOG_INFO << getServerContext()->getServerUri() << ": System user has logged.";
}

///报单录入请求响应
void CTPOTCTradeProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, _systemUser.getSessionId());
		int ret = _autoOrderMgr.OnOrderUpdated(*orderptr);
		DispatchUserMessage(MSG_ID_ORDER_NEW, nRequestID, orderptr->UserID(), orderptr);
	}
}

void CTPOTCTradeProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		if (pInputOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			auto orderptr = CTPUtility::ParseRawOrder(pInputOrderAction, pRspInfo);
			int ret = _autoOrderMgr.OnOrderUpdated(*orderptr);
			DispatchUserMessage(MSG_ID_ORDER_CANCEL, nRequestID, orderptr->UserID(), orderptr);
		}
	}
}

void CTPOTCTradeProcessor::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrder(pOrder);
		int ret = _autoOrderMgr.OnOrderUpdated(*orderptr);
		if (ret == 0)
		{
			DispatchUserMessage(CTPUtility::ParseMessageID(orderptr->OrderStatus), pOrder->RequestID, orderptr->UserID(), orderptr);
		}
	}
}

void CTPOTCTradeProcessor::RegisterLoggedSession(IMessageSession * pMessageSession)
{
	if (pMessageSession->getLoginTimeStamp() && _isLogged)
	{
		if (auto userInfoPtr = pMessageSession->getUserInfo())
		{
			userInfoPtr->setFrontId(_systemUser.getFrontId());
			userInfoPtr->setSessionId(_systemUser.getSessionId());
			_userSessionCtn_Ptr->add(userInfoPtr->getUserId(), pMessageSession);
		}
	}
}

void CTPOTCTradeProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		if (auto trdDO_Ptr = CTPUtility::ParseRawTrade(pTrade))
		{
			if (auto order_ptr = _autoOrderMgr.FindOrder(trdDO_Ptr->OrderID))
			{
				trdDO_Ptr->SetUserID(order_ptr->PortfolioID());
				trdDO_Ptr->SetPortfolioID(order_ptr->PortfolioID());
			}
			TradeDAO::SaveExchangeTrade(*trdDO_Ptr);
			DispatchUserMessage(MSG_ID_TRADE_RTN, 0, pTrade->UserID, trdDO_Ptr);
		}
	}
}