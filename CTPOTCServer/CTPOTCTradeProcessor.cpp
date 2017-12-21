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
	SendDataObject(getMessageSession(), MSG_ID_TRADE_RTN, 0, tradeDO);

	if (auto position_ptr = GetOTCOrderManager().GetPositionContext().GetPosition(tradeDO->UserID(), tradeDO->PortfolioID(), *tradeDO))
	{
		SendDataObject(getMessageSession(), MSG_ID_POSITION_UPDATED, 0, position_ptr);
	}
}

OrderDO_Ptr CTPOTCTradeProcessor::CreateOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;

	if (auto tdApiProxy = _rawAPI->TdAPIProxy())
	{
		// 端登成功,发出报单录入请求
		CThostFtdcInputOrderField req{};

		auto& userInfo = getMessageSession()->getUserInfo();

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
		CThostFtdcInputOrderActionField req{};

		auto& userInfo = getMessageSession()->getUserInfo();

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


//CTP API

///报单录入请求响应
void CTPOTCTradeProcessor::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrder)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this))
		{
			auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, getMessageSession()->getUserInfo().getSessionId());
			pWorkerProc->GetAutoOrderManager().OnMarketOrderUpdated(*orderptr, this);
			pWorkerProc->GetHedgeOrderManager().OnMarketOrderUpdated(*orderptr, this);

			if (CTPUtility::HasError(pRspInfo))
			{
				StrategyContractDO_Ptr strategy_ptr;
				pWorkerProc->PricingDataContext()->GetStrategyMap()->find(*orderptr, strategy_ptr);
				if (strategy_ptr && orderptr->TIF != OrderTIFType::IOC)
				{
					strategy_ptr->AutoOrderSettings.LimitOrderCounter--;
					strategy_ptr->BidEnabled = false;
					strategy_ptr->AskEnabled = false;
					strategy_ptr->Hedging = false;
					if (auto mdWorker = pWorkerProc->GetMDWorkerProcessor())
						mdWorker->DispatchUserMessage(MSG_ID_MODIFY_STRATEGY, 0, strategy_ptr->UserID(), strategy_ptr);
				}

				SendDataObject(getMessageSession(), MSG_ID_ORDER_NEW, 0, orderptr);
			}
		}
	}
}

void CTPOTCTradeProcessor::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction)
	{
		if (pInputOrderAction->ActionFlag == THOST_FTDC_AF_Delete)
		{
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this))
			{
				auto orderptr = CTPUtility::ParseRawOrder(pInputOrderAction, pRspInfo);
				pWorkerProc->GetAutoOrderManager().OnMarketOrderUpdated(*orderptr, this);
				pWorkerProc->GetHedgeOrderManager().OnMarketOrderUpdated(*orderptr, this);
			}
		}
	}
}

///报单录入错误回报
void CTPOTCTradeProcessor::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	if (pInputOrder)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this))
		{
			if (auto orderptr = CTPUtility::ParseRawOrder(pInputOrder, pRspInfo, getMessageSession()->getUserInfo().getSessionId()))
			{
				pWorkerProc->GetAutoOrderManager().OnMarketOrderUpdated(*orderptr, this);
				pWorkerProc->GetHedgeOrderManager().OnMarketOrderUpdated(*orderptr, this);

				StrategyContractDO_Ptr strategy_ptr;
				pWorkerProc->PricingDataContext()->GetStrategyMap()->find(*orderptr, strategy_ptr);
				if (strategy_ptr && orderptr->TIF != OrderTIFType::IOC)
				{
					strategy_ptr->AutoOrderSettings.LimitOrderCounter--;
					strategy_ptr->BidEnabled = false;
					strategy_ptr->AskEnabled = false;
					strategy_ptr->Hedging = false;
					if(auto mdWorker = pWorkerProc->GetMDWorkerProcessor())
						mdWorker->DispatchUserMessage(MSG_ID_MODIFY_STRATEGY, 0, strategy_ptr->UserID(), strategy_ptr);
				}

				SendDataObject(getMessageSession(), MSG_ID_ORDER_NEW, 0, orderptr);
			}
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
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this))
			{
				if (auto orderptr = CTPUtility::ParseRawOrder(pOrderAction, pRspInfo))
				{
					pWorkerProc->GetAutoOrderManager().OnMarketOrderUpdated(*orderptr, this);
					pWorkerProc->GetHedgeOrderManager().OnMarketOrderUpdated(*orderptr, this);
				}
			}
		}
	}
}

void CTPOTCTradeProcessor::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this))
		{
			bool sendNotification = false;

			if (pOrder->TimeCondition != THOST_FTDC_TC_IOC)
			{
				if (int orderSysId = CTPUtility::ToUInt64(pOrder->OrderSysID))
				{
					sendNotification = !pWorkerProc->GetUserOrderContext().FindOrder(orderSysId);
				}
			}

			auto orderptr = pWorkerProc->RefineOrder(pOrder);

			StrategyContractDO_Ptr strategy_ptr;

			if (sendNotification)
			{
				int limitOrders = pWorkerProc->GetUserOrderContext().GetLimitOrderCount(orderptr->InstrumentID());

				if (pWorkerProc->PricingDataContext()->GetStrategyMap()->find(*orderptr, strategy_ptr))
				{
					if (strategy_ptr->AutoOrderSettings.LimitOrderCounter < limitOrders)
					{
						strategy_ptr->AutoOrderSettings.LimitOrderCounter = limitOrders;
					}
				}
			}

			// update auto orders
			int ret = pWorkerProc->GetAutoOrderManager().OnMarketOrderUpdated(*orderptr, this);
			if (ret != 0)
			{
				if (!strategy_ptr)
					pWorkerProc->PricingDataContext()->GetStrategyMap()->find(*orderptr, strategy_ptr);

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
			}

			// update hedge orders
			ret = pWorkerProc->GetHedgeOrderManager().OnMarketOrderUpdated(*orderptr, this);

			if (sendNotification)
			{
				if (!strategy_ptr)
					pWorkerProc->PricingDataContext()->GetStrategyMap()->find(*orderptr, strategy_ptr);

				if (strategy_ptr)
					if (auto mdWorker = pWorkerProc->GetMDWorkerProcessor())
						mdWorker->DispatchUserMessage(MSG_ID_MODIFY_STRATEGY, 0, strategy_ptr->UserID(), strategy_ptr);
			}
		}
	}
}


OTCOrderManager & CTPOTCTradeProcessor::GetOTCOrderManager(void)
{
	auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this);
	return pWorkerProc->GetOTCOrderManager();
}

AutoOrderManager & CTPOTCTradeProcessor::GetAutoOrderManager(void)
{
	auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this);
	return pWorkerProc->GetAutoOrderManager();
}

HedgeOrderManager & CTPOTCTradeProcessor::GetHedgeOrderManager(void)
{
	auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this);
	return pWorkerProc->GetHedgeOrderManager();
}

std::shared_ptr<CTPOTCTradeProcessor> CTPOTCTradeProcessor::Shared_This()
{
	return std::static_pointer_cast<CTPOTCTradeProcessor>(shared_from_this());
}

void CTPOTCTradeProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		if (getMessageSession()->getUserInfo().getUserId() == pTrade->UserID)
		{
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this))
			{
				if (auto trdDO_Ptr = pWorkerProc->RefineTrade(pTrade))
				{
					pWorkerProc->PushToLogQueue(trdDO_Ptr);
					pWorkerProc->UpdatePosition(trdDO_Ptr);
				}
			}
		}
	}
}

void CTPOTCTradeProcessor::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast)
		DataLoadMask |= CTPProcessor::POSITION_DATA_LOADED;

	if (pInvestorPosition)
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCTradeWorkerProcessor>(this))
		{
			auto& userId = getMessageSession()->getUserInfo().getUserId();

			auto position = CTPUtility::ParseRawPosition(pInvestorPosition, userId);

			pWorkerProc->UpdateSysYdPosition(userId, position);

			if (!pWorkerProc->IsLoadPositionFromDB())
			{
				if (position->ExchangeID() == EXCHANGE_SHFE)
				{
					if (pInvestorPosition->PositionDate == THOST_FTDC_PSD_Today)
					{
						position = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position, false, false);
					}
					else
					{
						position = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position, true, false);
					}
				}
				else
					position = pWorkerProc->GetUserPositionContext()->UpsertPosition(userId, *position, false, true);
			}
		}
	}
}