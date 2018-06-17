/***********************************************************************
 * Module:  XTOTCTradeProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月27日 22:51:43
 * Purpose: Implementation of the class XTOTCTradeProcessor
 ***********************************************************************/

#include "XTOTCTradeProcessor.h"
#include "XTOTCTradeWorkerProcessor.h"

#include "XTUtility.h"
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

#include "../riskmanager/RiskUtil.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"
#include "../message/MessageUtility.h"
#include "../CTPOTCServer/CTPOTCWorkerProcessor.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPTradeProcessor::~CTPTradeProcessor()
 // Purpose:    Implementation of CTPTradeProcessor::~CTPTradeProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

XTOTCTradeProcessor::XTOTCTradeProcessor()
{
}

XTOTCTradeProcessor::XTOTCTradeProcessor(const XTRawAPI_Ptr & rawAPI)
	: XTTradeProcessor(rawAPI)
{
}

XTOTCTradeProcessor::~XTOTCTradeProcessor()
{
	LOG_DEBUG << __FUNCTION__;
}

void XTOTCTradeProcessor::OnTraded(const TradeRecordDO_Ptr& tradeDO)
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

XTOTCTradeWorkerProcessor* XTOTCTradeProcessor::getWorkerProcessor()
{
	return MessageUtility::WorkerProcessorPtr<XTOTCTradeWorkerProcessor>(getMessageSession()->LockMessageProcessor());
}


OTCOrderManager & XTOTCTradeProcessor::GetOTCOrderManager(void)
{
	return getWorkerProcessor()->GetOTCOrderManager();
}

AutoOrderManager & XTOTCTradeProcessor::GetAutoOrderManager(void)
{
	return getWorkerProcessor()->GetAutoOrderManager();
}

HedgeOrderManager & XTOTCTradeProcessor::GetHedgeOrderManager(void)
{
	return getWorkerProcessor()->GetHedgeOrderManager();
}


OrderDO_Ptr XTOTCTradeProcessor::CreateOrder(const OrderRequestDO& orderReq)
{
	if (auto tdApiProxy = std::static_pointer_cast<CTPRawAPI>(_rawAPI)->TdAPIProxy())
	{
		auto& userInfo = getMessageSession()->getUserInfo();

		ProductType productType;
		InstrumentCache& cache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);
		if (auto* pInstrument = cache.QueryInstrumentOrAddById(orderReq.InstrumentID()))
		{
			productType = pInstrument->ProductType;
		}

		COrdinaryOrder order{};

		// 资金账号，必填参数。不填会被api打回，并且通过onOrder反馈失败
		std::strncpy(order.m_strAccountID, userInfo.getInvestorId().data(), sizeof(order.m_strAccountID));

		// 单笔超价百分比，选填字段。默认为0
		order.m_dSuperPriceRate = 0;

		// 报单市场。必填字段。股票市场有"CFFEX"/"SHFE"/"DCE"/"CZCE"，如果填空或填错都会被api直接打回
		std::strncpy(order.m_strMarket, orderReq.ExchangeID().data(), sizeof(order.m_strMarket));

		// 报单合约代码，必填字段。
		std::strncpy(order.m_strInstrument, orderReq.InstrumentID().data(), sizeof(order.m_strInstrument));

		// 报单委托量，必填字段。默认int最大值，填0或不填会被api打回
		order.m_nVolume = orderReq.Volume;

		// 报单委托类型。必填字段。根据相应的业务选择，具体请参考XtDataType.h，默认为无效值(OPT_INVALID)。不填会被api打回
		order.m_eOperationType = XTUtility::GetOperationType(productType, orderReq.Direction, orderReq.OpenClose);

		// 报单价格类型，必填字段。默认为无效(PTRP_INVALID)，具体可参考XtDataType.h
		order.m_ePriceType = PRTP_HANG;

		// 报单价格，默认为double最大值。当价格类型m_ePriceType为指定价PRTP_FIX时，必填字段。当价格类型为其他时填了也没用
		order.m_dPrice = orderReq.LimitPrice;

		// 投机套保标志，选填字段。有"投机"/"套利"/"套保"方式。除期货三个方式都可选之外都是填“投机”。默认为“投机”
		order.m_eHedgeFlag = HEDGE_FLAG_SPECULATION;

		int reqId = (int)orderReq.OrderID;

		OrderReqCache::Insert(reqId, orderReq);

		std::static_pointer_cast<XTRawAPI>(_rawAPI)->get()->order(&order, reqId);
	}

	return std::make_shared<OrderDO>(orderReq);
}

OrderDO_Ptr XTOTCTradeProcessor::CancelOrder(const OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;

	if (auto tdApiProxy = std::static_pointer_cast<XTRawAPI>(_rawAPI))
	{
		auto& userInfo = getMessageSession()->getUserInfo();

		if (orderReq.OrderSysID != 0)
		{
			char orderSysId[sizeof(uint64_t) + 1];
			std::snprintf(orderSysId, sizeof(orderSysId), FMT_ORDERSYSID, orderReq.OrderSysID);
			std::static_pointer_cast<XTRawAPI>(_rawAPI)->get()->cancelOrder(userInfo.getInvestorId().data(), orderSysId, orderReq.ExchangeID().data(),
				orderReq.InstrumentID().data(), orderReq.SerialId);
		}
		else
		{
			std::static_pointer_cast<XTRawAPI>(_rawAPI)->get()->cancel(orderReq.OrderID, orderReq.SerialId);
		}
	}

	return ret;
}

uint32_t XTOTCTradeProcessor::GetSessionId(void)
{
	return getMessageSession()->getUserInfo().getSessionId();
}
