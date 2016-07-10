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

#include "../utility/LiteLogger.h"


CTPOTCTradeProcessor::CTPOTCTradeProcessor(const std::map<std::string, std::string>& configMap,
	IPricingDataContext* pricingCtx)
	: CTPTradeWorkerProcessor(configMap),
	_pricingCtx(pricingCtx),
	_otcOrderMgr(this, pricingCtx),
	_autoOrderMgr(this, pricingCtx)
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
	DEBUG_INFO(__FUNCTION__);
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


void CTPOTCTradeProcessor::TriggerOrderUpdating(const StrategyContractDO& strategyDO)
{
	if (auto updatedOrders = _autoOrderMgr.UpdateOrderByStrategy(strategyDO))
	{
		for (auto& order : *updatedOrders)
		{
			auto orderptr = std::make_shared<OrderDO>(order);
			DispatchUserMessage(MSG_ID_ORDER_UPDATE, orderptr->UserID(), orderptr);
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

		DispatchUserMessage(MSG_ID_ORDER_CANCEL, orderptr->UserID(), orderptr);
	}
}

void CTPOTCTradeProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		auto orderptr = CTPUtility::ParseRawOrderAction(pInputOrderAction, pRspInfo, OrderStatus::CANCEL_REJECTED);

		int ret = _autoOrderMgr.OnOrderUpdated(*orderptr);

		DispatchUserMessage(MSG_ID_ORDER_CANCEL, orderptr->UserID(), orderptr);
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

			DispatchUserMessage(msgId, orderptr->UserID(), orderptr);
		}
	}
}