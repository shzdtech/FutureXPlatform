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
#include "../message/SysParam.h"
#include "../message/AppContext.h"

#include "../litelogger/LiteLogger.h"


CTPOTCTradeProcessor::CTPOTCTradeProcessor(
	const std::map<std::string, std::string>& configMap,
	IServerContext* pServerCtx,
	IPricingDataContext* pricingDataCtx)
	: CTPTradeWorkerProcessor(configMap, pServerCtx),
	_pricingDataCtx(pricingDataCtx),
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

void CTPOTCTradeProcessor::Initialize(void)
{
	if (!_rawAPI->TrdAPI) {
		_rawAPI->TrdAPI = CThostFtdcTraderApi::CreateFtdcTraderApi();
		_rawAPI->TrdAPI->RegisterSpi(this);
		_rawAPI->TrdAPI->RegisterFront(const_cast<char*> (_systemUser.getServer().data()));
		_rawAPI->TrdAPI->SubscribePrivateTopic(THOST_TERT_RESUME);
		_rawAPI->TrdAPI->SubscribePublicTopic(THOST_TERT_RESUME);
		_rawAPI->TrdAPI->Init();
		std::this_thread::sleep_for(std::chrono::seconds(1)); //wait 1 secs for connecting to CTP server
	}
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
	std::strcpy(req.BrokerID, _systemUser.getBrokerId().data());
	//投资者代码
	std::strcpy(req.InvestorID, _systemUser.getInvestorId().data());
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

	_rawAPI->TrdAPI->ReqOrderInsert(&req, AppContext::GenNextSeq());

	return CTPUtility::ParseRawOrderInput(&req, nullptr, _systemUser.getSessionId());
}


OrderDO_Ptr CTPOTCTradeProcessor::CancelOrder(OrderRequestDO& orderInfo)
{
	CThostFtdcInputOrderActionField req{};

	req.ActionFlag = THOST_FTDC_AF_Delete;
	//经纪公司代码
	std::strcpy(req.BrokerID, _systemUser.getBrokerId().data());
	std::strcpy(req.InvestorID, _systemUser.getUserId().data());
	std::strcpy(req.UserID, orderInfo.UserID().data());
	if (orderInfo.OrderSysID != 0)
	{
		std::strcpy(req.ExchangeID, orderInfo.ExchangeID().data());
		std::sprintf(req.OrderSysID, FMT_PADDING_ORDERSYSID, orderInfo.OrderSysID);
	}
	else
	{
		req.SessionID = _systemUser.getSessionId();
		req.FrontID = _systemUser.getFrontId();
		std::strcpy(req.InstrumentID, orderInfo.InstrumentID().data());
		std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, orderInfo.OrderID);
	}

	_rawAPI->TrdAPI->ReqOrderAction(&req, AppContext::GenNextSeq());
	
	req.SessionID = _systemUser.getSessionId();
	return CTPUtility::ParseRawOrderInputAction(&req, nullptr);
}

IPricingDataContext * CTPOTCTradeProcessor::GetPricingContext(void)
{
	return _pricingDataCtx;
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
///报单操作请求响应

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
}

///报单录入请求响应
void CTPOTCTradeProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		auto orderptr = CTPUtility::ParseRawOrderInput(pInputOrder, pRspInfo, _systemUser.getSessionId());

		int ret = _autoOrderMgr.OnOrderUpdated(*orderptr);

		DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
	}
}

void CTPOTCTradeProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		auto orderptr = CTPUtility::ParseRawOrderInputAction(pInputOrderAction, pRspInfo);

		int ret = _autoOrderMgr.OnOrderUpdated(*orderptr);

		DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
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
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, 0, orderptr->UserID(), orderptr);
		}
	}
}