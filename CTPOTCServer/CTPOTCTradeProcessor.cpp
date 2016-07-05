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