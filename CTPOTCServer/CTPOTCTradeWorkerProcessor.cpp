/***********************************************************************
 * Module:  CTPOTCTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class CTPOTCTradeWorkerProcessor
 ***********************************************************************/

#include "CTPOTCTradeWorkerProcessor.h"
#include "CTPOTCTradeSAProcessor.h"
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
#include "../ordermanager/OrderReqCache.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"


CTPOTCTradeWorkerProcessor::CTPOTCTradeWorkerProcessor(
	IServerContext* pServerCtx,
	const IPricingDataContext_Ptr& pricingDataCtx,
	const IUserPositionContext_Ptr& positionCtx)
	: CTPOTCTradeWorkerProcessorBase(pServerCtx, pricingDataCtx, positionCtx),
	CTPTradeWorkerProcessor(pServerCtx, positionCtx)
{
	_logTrades = false;
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradeWorkerProcessor::~CTPOTCTradeWorkerProcessor()
{
}

CTPTradeWorkerSAProcessor_Ptr CTPOTCTradeWorkerProcessor::CreateSAProcessor()
{
	return std::make_shared<CTPOTCTradeSAProcessor>(this);
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
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(orderReq.UserID(), [this, &orderReq, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CreateOrder(orderReq);
	});

	return ret;
}


OrderDO_Ptr CTPOTCTradeWorkerProcessor::CancelOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(orderReq.UserID(), [this, &orderReq, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelOrder(orderReq);
	});

	return ret;
}


OrderDO_Ptr CTPOTCTradeWorkerProcessor::CancelAutoOrder(const UserContractKey& userContractKey)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(userContractKey.UserID(), [this, &userContractKey, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelAutoOrder(userContractKey);
	});

	return ret;
}

OrderDO_Ptr CTPOTCTradeWorkerProcessor::CancelHedgeOrder(const PortfolioKey& portfolioKey)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(portfolioKey.UserID(), [this, &portfolioKey, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<CTPOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelHedgeOrder(portfolioKey);
	});

	return ret;
}

uint32_t CTPOTCTradeWorkerProcessor::GetSessionId(void)
{
	return _systemUser.getSessionId();
}

bool & CTPOTCTradeWorkerProcessor::Closing(void)
{
	return (bool)_closing;
}

SessionContainer_Ptr<std::string>& CTPOTCTradeWorkerProcessor::GetUserSessionContainer(void)
{
	return _userSessionCtn_Ptr;
}

IUserPositionContext_Ptr & CTPOTCTradeWorkerProcessor::GetUserPositionContext()
{
	return _userPositionCtx_Ptr;
}

UserTradeContext & CTPOTCTradeWorkerProcessor::GetUserTradeContext()
{
	return _userTradeCtx;
}

UserOrderContext & CTPOTCTradeWorkerProcessor::GetExchangeOrderContext(void)
{
	return _userOrderCtx;
}


//CTP API

void CTPOTCTradeWorkerProcessor::OnRspOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void CTPOTCTradeWorkerProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField * pInputOrderAction, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
}

void CTPOTCTradeWorkerProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField * pInputOrder, CThostFtdcRspInfoField * pRspInfo)
{
}

void CTPOTCTradeWorkerProcessor::OnErrRtnOrderAction(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRspInfo)
{
}

void CTPOTCTradeWorkerProcessor::OnRtnOrder(CThostFtdcOrderField * pOrder)
{
}


void CTPOTCTradeWorkerProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
}