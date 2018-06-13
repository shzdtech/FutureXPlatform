/***********************************************************************
 * Module:  CTPOTCTradeProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class CTPOTCTradeProcessor
 ***********************************************************************/

#include "CTPOTCTradeProcessor.h"
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

#include "../riskmanager/RiskUtil.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"
#include "../message/MessageUtility.h"
#include "CTPOTCWorkerProcessor.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeProcessor::~CTPTradeProcessor()
 // Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPOTCTradeProcessor::CTPOTCTradeProcessor()
{
}

CTPOTCTradeProcessor::CTPOTCTradeProcessor(const CTPRawAPI_Ptr & rawAPI)
	: CTPTradeProcessor(rawAPI)
{
}

CTPOTCTradeProcessor::~CTPOTCTradeProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

void CTPOTCTradeProcessor::OnTraded(const TradeRecordDO_Ptr& tradeDO)
{
	/*if (auto pWorkerProc = getWorkerProcessor())
	{
		pWorkerProc->DispatchUserMessage(MSG_ID_TRADE_RTN, 0, tradeDO->UserID(), tradeDO);

		if (auto position_ptr = GetOTCOrderManager().GetPositionContext().GetPosition(tradeDO->UserID(), tradeDO->PortfolioID(), *tradeDO))
		{
			pWorkerProc->DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, position_ptr->UserID(), position_ptr);
		}
	}*/
}

CTPOTCTradeWorkerProcessor* CTPOTCTradeProcessor::getWorkerProcessor()
{
	return MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(getMessageSession()->LockMessageProcessor());
}


OTCOrderManager & CTPOTCTradeProcessor::GetOTCOrderManager(void)
{
	return getWorkerProcessor()->GetOTCOrderManager();
}

AutoOrderManager & CTPOTCTradeProcessor::GetAutoOrderManager(void)
{
	return getWorkerProcessor()->GetAutoOrderManager();
}

HedgeOrderManager & CTPOTCTradeProcessor::GetHedgeOrderManager(void)
{
	return getWorkerProcessor()->GetHedgeOrderManager();
}


OrderDO_Ptr CTPOTCTradeProcessor::CreateOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;

	if (auto pWorkerProc = getWorkerProcessor())
	{
		try
		{
			RiskUtil::CheckRisk(orderReq, pWorkerProc->GetUserPositionContext().get(), pWorkerProc->GetAccountInfo(orderReq.UserID()).get());
		}
		catch (BizException& ex)
		{
			if (ex.ErrorCode() > RiskModelParams::WARNING)
			{
				if (auto mdWorker = pWorkerProc->GetMDWorkerProcessor())
					mdWorker->DispatchUserExceptionMessage(MSG_ID_ORDER_NEW, 0, orderReq.UserID(), ex);
				return ret;
			}
		}
	}

	if (auto tdApiProxy = _rawAPI->TdAPIProxy())
	{
		auto& userInfo = getMessageSession()->getUserInfo();

		// 端登成功,发出报单录入请求
		CThostFtdcInputOrderField req{};

		//经纪公司代码
		std::strncpy(req.BrokerID, userInfo.getBrokerId().data(), sizeof(req.BrokerID));
		//投资者代码
		std::strncpy(req.InvestorID, userInfo.getInvestorId().data(), sizeof(req.InvestorID));
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

		if (tdApiProxy->get()->ReqOrderInsert(&req, 0) == 0)
		{
			ret = CTPUtility::ParseRawOrder(&req, nullptr, orderReq.SessionID ? orderReq.SessionID : userInfo.getSessionId());
		}
		else
		{
			OrderPortfolioCache::Remove(orderReq.OrderID);
		}
	}

	return ret;
}

OrderDO_Ptr CTPOTCTradeProcessor::CancelOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;

	if (auto tdApiProxy = _rawAPI->TdAPIProxy())
	{
		auto& userInfo = getMessageSession()->getUserInfo();

		CThostFtdcInputOrderActionField req{};

		req.ActionFlag = THOST_FTDC_AF_Delete;
		//经纪公司代码
		std::strncpy(req.BrokerID, userInfo.getBrokerId().data(), sizeof(req.BrokerID));
		std::strncpy(req.InvestorID, userInfo.getUserId().data(), sizeof(req.InvestorID));
		std::strncpy(req.UserID, orderReq.UserID().data(), sizeof(req.UserID));

		if (orderReq.OrderSysID != 0)
		{
			std::strncpy(req.ExchangeID, orderReq.ExchangeID().data(), sizeof(req.ExchangeID));
			std::snprintf(req.OrderSysID, sizeof(req.OrderSysID), FMT_ORDERSYSID, orderReq.OrderSysID);
		}
		else
		{
			req.FrontID = userInfo.getFrontId();
			req.SessionID = orderReq.SessionID ? orderReq.SessionID : userInfo.getSessionId();
			std::strncpy(req.InstrumentID, orderReq.InstrumentID().data(), sizeof(req.InstrumentID));
			std::snprintf(req.OrderRef, sizeof(req.OrderRef), FMT_ORDERREF, orderReq.OrderID);
		}


		if (tdApiProxy->get()->ReqOrderAction(&req, AppContext::GenNextSeq()) == 0)
		{
			req.SessionID = orderReq.SessionID ? orderReq.SessionID : userInfo.getSessionId();
			ret = CTPUtility::ParseRawOrder(&req, nullptr);
		}
	}

	return ret;
}

uint32_t CTPOTCTradeProcessor::GetSessionId(void)
{
	return getMessageSession()->getUserInfo().getSessionId();
}
