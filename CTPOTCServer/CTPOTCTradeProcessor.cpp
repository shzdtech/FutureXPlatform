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

#include <glog/logging.h>

#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../message/SysParam.h"
#include "../message/AppContext.h"


CTPOTCTradeProcessor::CTPOTCTradeProcessor(const std::map<std::string, std::string>& configMap,
	IPricingDataContext* pricingCtx)
	: CTPTradeWorkerProcessor(configMap),
	_pricingCtx(pricingCtx),
	_otcOrderMgr(this, pricingCtx),
	_autoOrderMgr(this, pricingCtx),
	_orderNotifier(new SessionContainer<uint64_t>())
{
}

AutoOrderManager& CTPOTCTradeProcessor::GetAutoOrderManager(void)
{
	return _autoOrderMgr;
}

OTCOrderManager& CTPOTCTradeProcessor::GetOTCOrderManager(void)
{
	return _otcOrderMgr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradeProcessor::~CTPOTCTradeProcessor()
{
	DLOG(INFO) << __FUNCTION__ << std::endl;
	if (_rawAPI.TrdAPI)
		_rawAPI.TrdAPI->Release();
}

void CTPOTCTradeProcessor::Initialize(void)
{
	InstrmentsLoaded = true;
	CTPTradeWorkerProcessor::Initialize();
}

bool CTPOTCTradeProcessor::OnSessionClosing(void)
{
	auto pMap = _pricingCtx->GetStrategyMap();
	for (auto& it : *pMap)
	{
		auto& stragety = it.second;
		stragety.Enabled = false;
		stragety.Trading = false;
	}

	_otcOrderMgr.Reset();
	_autoOrderMgr.Reset();

	return true;
}

void CTPOTCTradeProcessor::DispatchMessage(const int msgId, const OrderDO* pOrderDO)
{
	ScopeLockContext sclcctx;
	if (auto pNotiferSet = _orderNotifier->getwithlock(pOrderDO->OrderID, sclcctx))
	{
		for (auto pSession : *pNotiferSet)
		{
			OnResponseProcMacro(pSession->getProcessor(), msgId, pOrderDO->SerialId, pOrderDO);
		}
	}
}

void CTPOTCTradeProcessor::RegisterOrderListener(const uint64_t orderID,
	IMessageSession* pMsgSession)
{
	_orderNotifier->add(orderID, pMsgSession);
}


void CTPOTCTradeProcessor::TriggerOrderUpdating(const StrategyContractDO& strategyDO)
{
	if (auto updatedOrders = _autoOrderMgr.UpdateOrderByStrategy(strategyDO))
	{
		for (auto& order : *updatedOrders)
		{
			DispatchMessage(MSG_ID_ORDER_UPDATE, &order);
		}
	}
}


int CTPOTCTradeProcessor::CreateOrder(const OrderDO& orderInfo, OrderStatus& currStatus)
{
	currStatus = OrderStatus::UNDEFINED;
	// 端登成功,发出报单录入请求
	CThostFtdcInputOrderField req;
	std::memset(&req, 0, sizeof(req));

	//经纪公司代码
	std::strcpy(req.BrokerID, _defaultUser.getBrokerId().data());
	//投资者代码
	std::strcpy(req.InvestorID, _defaultUser.getUserId().data());
	// 合约代码
	std::strcpy(req.InstrumentID, orderInfo.InstrumentID().data());
	///报单引用
	std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, orderInfo.OrderID);
	// 用户代码
	std::strcpy(req.UserID, orderInfo.UserID().data());
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

	currStatus = OrderStatus::OPENNING;

	return _rawAPI.TrdAPI->ReqOrderInsert(&req, AppContext::GenNextSeq());
}


int CTPOTCTradeProcessor::CancelOrder(const OrderDO& orderInfo, OrderStatus& currStatus)
{
	currStatus = OrderStatus::UNDEFINED;

	CThostFtdcInputOrderActionField req;
	std::memset(&req, 0x0, sizeof(req));

	req.ActionFlag = THOST_FTDC_AF_Delete;
	//经纪公司代码
	std::strcpy(req.BrokerID, _defaultUser.getBrokerId().data());
	std::strcpy(req.InvestorID, _defaultUser.getUserId().data());
	std::strcpy(req.UserID, orderInfo.UserID().data());
	if (orderInfo.OrderSysID != 0)
	{
		std::strcpy(req.ExchangeID, orderInfo.ExchangeID().data());
		std::sprintf(req.OrderSysID, FMT_PADDING_ORDERSYSID, orderInfo.OrderSysID);
	}
	else
	{
		auto pUser = getSession()->getUserInfo();
		req.SessionID = pUser->getSessionId();
		req.FrontID = pUser->getFrontId();
		std::strcpy(req.InstrumentID, orderInfo.InstrumentID().data());
		std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, orderInfo.OrderID);
	}

	currStatus = OrderStatus::CANCELING;

	return _rawAPI.TrdAPI->ReqOrderAction(&req, AppContext::GenNextSeq());
}




//CTP API
///报单操作请求响应

///登录请求响应
void CTPOTCTradeProcessor::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspUserLogin && !CTPUtility::HasError(pRspInfo))
	{
		auto pUser = getSession()->getUserInfo();
		pUser->setSessionId(pRspUserLogin->SessionID);
		pUser->setFrontId(pRspUserLogin->FrontID);
		_autoOrderMgr.Reset();
	}
}

///报单录入请求响应
void CTPOTCTradeProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrderInput(pInputOrder, pRspInfo, OrderStatus::OPEN_REJECTED);

		int ret = _autoOrderMgr.OnOrderUpdated(*orderptr);

		DispatchMessage(MSG_ID_ORDER_CANCEL, orderptr.get());
	}
}

void CTPOTCTradeProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		auto orderptr = CTPUtility::ParseRawOrderAction(pInputOrderAction, pRspInfo, OrderStatus::CANCEL_REJECTED);

		int ret = _autoOrderMgr.OnOrderUpdated(*orderptr);

		DispatchMessage(MSG_ID_ORDER_CANCEL, orderptr.get());
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
			int msgId;
			switch (orderptr->OrderStatus)
			{
			case OrderStatus::CANCELED:
			case OrderStatus::CANCEL_REJECTED:
				msgId = MSG_ID_ORDER_CANCEL;
				break;
			case OrderStatus::OPENNING:
			case OrderStatus::OPEN_REJECTED:
				msgId = MSG_ID_ORDER_NEW;
				break;
			case OrderStatus::PARTIAL_TRADING:
			case OrderStatus::ALL_TRADED:
				msgId = MSG_ID_ORDER_UPDATE;
				break;
			default:
				return;
			}

			DispatchMessage(msgId, orderptr.get());
		}
	}
}