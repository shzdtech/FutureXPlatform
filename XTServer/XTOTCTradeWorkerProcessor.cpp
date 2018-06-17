/***********************************************************************
 * Module:  XTOTCTradeWorkerProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class XTOTCTradeWorkerProcessor
 ***********************************************************************/

#include "XTOTCTradeWorkerProcessor.h"
#include "XTUtility.h"
#include "../CTPServer/CTPConstant.h"
#include "../CTPServer/CTPMapping.h"
#include "../dataobject/TradeRecordDO.h"
#include "../common/Attribute_Key.h"
#include "../systemsettings/AppContext.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../message/MessageSession.h"
#include "../bizutility/ExchangeRouterTable.h"
#include "../bizutility/PositionPortfolioMap.h"

#include "../databaseop/VersionDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/TradeDAO.h"
#include "../ordermanager/OrderSeqGen.h"
#include "../ordermanager/OrderReqCache.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"


XTOTCTradeWorkerProcessor::XTOTCTradeWorkerProcessor(
	IServerContext* pServerCtx,
	const IPricingDataContext_Ptr& pricingDataCtx,
	const IUserPositionContext_Ptr& positionCtx)
	: CTPOTCTradeWorkerProcessorBase(pServerCtx, pricingDataCtx, positionCtx),
	XTTradeWorkerProcessor(pServerCtx, positionCtx)
{
	_logTrades = false;
	_msgsession_ptr = std::make_shared<MessageSession>();
}


////////////////////////////////////////////////////////////////////////
// Name:       CTPTradeProcessor::~CTPTradeProcessor()
// Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

XTOTCTradeWorkerProcessor::~XTOTCTradeWorkerProcessor()
{
}

void XTOTCTradeWorkerProcessor::OnTraded(const TradeRecordDO_Ptr& tradeDO)
{
	DispatchUserMessage(MSG_ID_TRADE_RTN, 0, tradeDO->UserID(), tradeDO);

	if (auto position_ptr = GetOTCOrderManager().GetPositionContext().GetPosition(tradeDO->UserID(), tradeDO->PortfolioID(), *tradeDO))
	{
		DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, tradeDO->UserID(), position_ptr);
	}
}


OrderDO_Ptr XTOTCTradeWorkerProcessor::CreateOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(orderReq.UserID(), [this, &orderReq, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<XTOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CreateOrder(orderReq);
	});

	return ret;
}


OrderDO_Ptr XTOTCTradeWorkerProcessor::CancelOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(orderReq.UserID(), [this, &orderReq, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<XTOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelOrder(orderReq);
	});

	return ret;
}


OrderDO_Ptr XTOTCTradeWorkerProcessor::CancelAutoOrder(const UserContractKey& userContractKey)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(userContractKey.UserID(), [this, &userContractKey, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<XTOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelAutoOrder(userContractKey);
	});

	return ret;
}

OrderDO_Ptr XTOTCTradeWorkerProcessor::CancelHedgeOrder(const PortfolioKey& portfolioKey)
{
	OrderDO_Ptr ret;
	_userSessionCtn_Ptr->forfirst(portfolioKey.UserID(), [this, &portfolioKey, &ret](const IMessageSession_Ptr& msgSession)
	{
		if (auto otcTradeProc_Ptr = std::static_pointer_cast<XTOTCTradeProcessor>(msgSession->LockMessageProcessor()))
			ret = otcTradeProc_Ptr->CancelHedgeOrder(portfolioKey);
	});

	return ret;
}

uint32_t XTOTCTradeWorkerProcessor::GetSessionId(void)
{
	return _systemUser.getSessionId();
}

bool & XTOTCTradeWorkerProcessor::Closing(void)
{
	return (bool)_closing;
}

SessionContainer_Ptr<std::string>& XTOTCTradeWorkerProcessor::GetUserSessionContainer(void)
{
	return _userSessionCtn_Ptr;
}

IUserPositionContext_Ptr & XTOTCTradeWorkerProcessor::GetUserPositionContext()
{
	return _userPositionCtx_Ptr;
}

UserTradeContext & XTOTCTradeWorkerProcessor::GetUserTradeContext()
{
	return _userTradeCtx;
}

UserOrderContext & XTOTCTradeWorkerProcessor::GetExchangeOrderContext(void)
{
	return _userOrderCtx;
}