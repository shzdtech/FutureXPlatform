/***********************************************************************
 * Module:  XTUtility.cpp
 * Author:  milk
 * Modified: 2015年7月16日 21:32:43
 * Purpose: Implementation of the class XTUtility
 ***********************************************************************/

#include "XTUtility.h"
#include "XTConstant.h"
#include "../message/BizError.h"
#include <boost/locale/encoding.hpp>
#include "../utility/commonconst.h"
#include "../common/BizErrorIDs.h"
#include "../bizutility/ContractCache.h"
#include "../bizutility/PositionPortfolioMap.h"
#include "../utility/stringutility.h"
#include "../ordermanager/OrderSeqGen.h"
#include "../databaseop/ContractDAO.h"
#include "../litelogger/LiteLogger.h"

#include "../message/DefMessageID.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       XTUtility::CheckError()
 // Purpose:    Implementation of XTUtility::CheckError()
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

void XTUtility::CheckError(const void* pRspInfo)
{
	auto pRsp = (CThostFtdcRspInfoField*)pRspInfo;
	if (HasError(pRsp))
	{	
		throw ApiException(pRsp->ErrorID,
			std::move(boost::locale::conv::to_utf<char>(pRsp->ErrorMsg, CHARSET_GB2312)));
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       XTUtility::HasError(void* pRespInfo)
// Purpose:    Implementation of XTUtility::HasError()
// Parameters:
// - pRespInfo
// Return:     bool
////////////////////////////////////////////////////////////////////////

void XTUtility::CheckTradeInit(XTRawAPI * pCtpAPI)
{
	if (!pCtpAPI)
	{
		throw UserException("Not login to CTP trade server ...");
	}
}

bool XTUtility::HasTradeInit(XTRawAPI * pCtpAPI)
{
	return pCtpAPI;
}

void XTUtility::CheckNotFound(const void * pRspData)
{
	if (!pRspData)
	{
		throw NotFoundException();
	}
}

bool XTUtility::HasError(CThostFtdcRspInfoField* pRspInfo)
{
	bool ret = false;

	if (pRspInfo)
	{
		if (pRspInfo->ErrorID)
		{
			ret = true;
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTUtility::CheckReturnError(int rtncode)
// Purpose:    Implementation of XTUtility::CheckReturnError()
// Parameters:
// - rtncode
// Return:     void
////////////////////////////////////////////////////////////////////////

void XTUtility::CheckReturnError(const int rtnCode)
{
	if (auto bizError_Ptr = HasReturnError(rtnCode))
		throw *bizError_Ptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTUtility::HasReturnError(int rtnCode, BizError& bizError)
// Purpose:    Implementation of XTUtility::HasReturnError()
// Parameters:
// - rtnCode
// - bizError
// Return:     bool
////////////////////////////////////////////////////////////////////////

std::shared_ptr<ApiException> XTUtility::HasReturnError(const int rtnCode)
{
	switch (rtnCode)
	{
	case 0:
		return nullptr;
	case -1:
		return std::make_shared<ApiException>(CONNECTION_ERROR, "Cannot connect to CTP server");
	case -2:
		return std::make_shared<ApiException>(NO_PERMISSION, "Unresolved requests exeeding permission");
	case -3:
		return std::make_shared<ApiException>(TOO_MANY_REQUEST, "The number of requests sent per second exeeding permission");
	default:
		break;
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       XTUtility::HasReturnError(int rtnCode, BizError& bizError)
// Purpose:    Implementation of XTUtility::HasReturnError()
// Parameters:
// - rtnCode
// - bizError
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool XTUtility::IsOrderActive(TThostFtdcOrderStatusType status)
{
	bool ret = true;

	switch (status)
	{
	case THOST_FTDC_OST_AllTraded:
	case THOST_FTDC_OST_PartTradedNotQueueing:
	case THOST_FTDC_OST_NoTradeNotQueueing:
	case THOST_FTDC_OST_Canceled:
		ret = false;
		break;
	default:
		break;
	}

	return ret;
}


OrderStatusType XTUtility::CheckOrderStatus(TThostFtdcOrderStatusType status, TThostFtdcOrderSubmitStatusType submitStatus)
{
	OrderStatusType ret = OrderStatusType::UNDEFINED;

	switch (status)
	{
	case THOST_FTDC_OST_Canceled:
		ret = OrderStatusType::CANCELED;
		break;
	case THOST_FTDC_OST_AllTraded:
		ret = OrderStatusType::ALL_TRADED;
		break;
	case THOST_FTDC_OST_NoTradeQueueing:
		ret = OrderStatusType::OPENED;
		break;
	case THOST_FTDC_OST_NoTradeNotQueueing:
		ret = OrderStatusType::OPEN_REJECTED;
		break;
	case THOST_FTDC_OST_PartTradedQueueing:
		ret = OrderStatusType::PARTIAL_TRADING;
		break;
	case THOST_FTDC_OST_PartTradedNotQueueing:
		ret = OrderStatusType::PARTIAL_TRADED;
		break;
	default:
		switch (submitStatus)
		{
		case THOST_FTDC_OSS_InsertSubmitted:
			ret = OrderStatusType::OPENING;
			break;
		case THOST_FTDC_OSS_InsertRejected:
			ret = OrderStatusType::OPEN_REJECTED;
			break;
		case THOST_FTDC_OSS_CancelSubmitted:
			ret = OrderStatusType::CANCELING;
			break;
		case THOST_FTDC_OSS_CancelRejected:
			ret = OrderStatusType::CANCEL_REJECTED;
			break;
		default:
			break;
		}
	}

	return ret;
}

OrderDO_Ptr XTUtility::ParseRawOrder(CThostFtdcOrderField *pOrder, OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
	{
		baseOrder.reset(new OrderDO(ToUInt64(pOrder->OrderRef),	pOrder->ExchangeID, pOrder->InstrumentID, pOrder->UserID));
	}

	auto pDO = baseOrder.get();
	pDO->Direction = (pOrder->Direction == THOST_FTDC_D_Buy) ?
		DirectionType::BUY : DirectionType::SELL;
	pDO->OpenClose = (OrderOpenCloseType)(pOrder->CombOffsetFlag[0] - THOST_FTDC_OF_Open);
	pDO->LimitPrice = pOrder->LimitPrice;
	pDO->Volume = pOrder->VolumeTotalOriginal;
	pDO->StopPrice = pOrder->StopPrice;
	pDO->OrderSysID = ToUInt64(pOrder->OrderSysID);
	pDO->Active = IsOrderActive(pOrder->OrderStatus);
	pDO->OrderStatus = CheckOrderStatus(pOrder->OrderStatus, pOrder->OrderSubmitStatus);
	pDO->VolumeTraded = pOrder->VolumeTraded;
	pDO->TIF = pOrder->TimeCondition == THOST_FTDC_TC_IOC ? OrderTIFType::IOC : OrderTIFType::GFD;
	pDO->VolCondition = (OrderVolType)(pOrder->VolumeCondition - THOST_FTDC_VC_AV);

	pDO->BrokerID = pOrder->BrokerID;
	pDO->InvestorID = pOrder->InvestorID;
	pDO->InsertDate = pOrder->InsertDate;
	pDO->InsertTime = pOrder->InsertTime;
	pDO->UpdateTime = pOrder->UpdateTime;
	pDO->CancelTime = pOrder->CancelTime;
	pDO->TradingDay = std::atoi(pOrder->TradingDay);
	pDO->Message = std::move(boost::locale::conv::to_utf<char>(pOrder->StatusMsg, CHARSET_GB2312));
	pDO->SessionID = pOrder->SessionID;

	return baseOrder;
}

OrderDO_Ptr XTUtility::ParseRawOrder(
	CThostFtdcInputOrderActionField *pOrderAction,
	CThostFtdcRspInfoField *pRsp,
	OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
	{
		baseOrder.reset(new OrderDO(ToUInt64(pOrderAction->OrderRef),
			pOrderAction->ExchangeID, pOrderAction->InstrumentID, pOrderAction->UserID));
	}

	auto pDO = baseOrder.get();
	pDO->OrderSysID = ToUInt64(pOrderAction->OrderSysID);
	pDO->LimitPrice = pOrderAction->LimitPrice;
	pDO->Active = pRsp == nullptr;
	pDO->OrderStatus = pRsp ? OrderStatusType::CANCEL_REJECTED : OrderStatusType::CANCELING;
	pDO->SessionID = pOrderAction->SessionID;

	pDO->BrokerID = pOrderAction->BrokerID;
	pDO->InvestorID = pOrderAction->InvestorID;

	if (pRsp)
	{
		pDO->ErrorCode = pRsp->ErrorID;
		pDO->Message = std::move(boost::locale::conv::to_utf<char>(pRsp->ErrorMsg, CHARSET_GB2312));
	}

	return baseOrder;
}

OrderDO_Ptr XTUtility::ParseRawOrder(
	CThostFtdcInputOrderField *pOrderInput,
	CThostFtdcRspInfoField *pRsp,
	int sessionID,
	OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
	{
		const char* pExchange = "";
		if (auto pInstument = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(pOrderInput->InstrumentID))
		{
			pExchange = pInstument->ExchangeID().data();
		}

		baseOrder.reset(new OrderDO(ToUInt64(pOrderInput->OrderRef),
			pExchange, pOrderInput->InstrumentID, pOrderInput->UserID));
	}

	auto pDO = baseOrder.get();
	pDO->SessionID = sessionID;
	pDO->Direction = (pOrderInput->Direction == THOST_FTDC_D_Buy) ? DirectionType::BUY : DirectionType::SELL;
	pDO->LimitPrice = pOrderInput->LimitPrice;
	pDO->Volume = pOrderInput->VolumeTotalOriginal;
	pDO->StopPrice = pOrderInput->StopPrice;
	pDO->Active = pRsp == nullptr;
	pDO->OrderStatus = pRsp ? OrderStatusType::OPEN_REJECTED : OrderStatusType::OPENING;

	pDO->BrokerID = pOrderInput->BrokerID;
	pDO->InvestorID = pOrderInput->InvestorID;
	//auto now = std::time(nullptr);
	//auto tm = std::localtime(&now);
	//char timebuf[20];
	//std::strftime(timebuf, sizeof(timebuf), "%Y%m%d %T", tm);
	//pDO->InsertTime = timebuf;

	if (pRsp) {
		pDO->ErrorCode = pRsp->ErrorID;
		pDO->Message = std::move(boost::locale::conv::to_utf<char>(pRsp->ErrorMsg, CHARSET_GB2312));
	}

	return baseOrder;
}

OrderDO_Ptr XTUtility::ParseRawOrder(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRsp, OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
	{
		baseOrder.reset(new OrderDO(ToUInt64(pOrderAction->OrderRef),
			pOrderAction->ExchangeID, pOrderAction->InstrumentID, pOrderAction->UserID));
	}

	auto pDO = baseOrder.get();

	pDO->OrderSysID = ToUInt64(pOrderAction->OrderSysID);
	pDO->LimitPrice = pOrderAction->LimitPrice;
	pDO->Active = pRsp == nullptr;
	pDO->OrderStatus = pRsp ? OrderStatusType::CANCEL_REJECTED : OrderStatusType::CANCELING;
	pDO->SessionID = pOrderAction->SessionID;

	pDO->BrokerID = pOrderAction->BrokerID;
	pDO->InvestorID = pOrderAction->InvestorID;

	if (pRsp)
	{
		pDO->ErrorCode = pRsp->ErrorID;
		pDO->Message = std::move(boost::locale::conv::to_utf<char>(pRsp->ErrorMsg, CHARSET_GB2312));
	}
	else
	{
		pDO->Message = std::move(boost::locale::conv::to_utf<char>(pOrderAction->StatusMsg, CHARSET_GB2312));
	}

	return baseOrder;
}

TradeRecordDO_Ptr XTUtility::ParseRawTrade(CThostFtdcTradeField * pTrade)
{
	TradeRecordDO_Ptr ret;
	if (pTrade)
	{
		auto pDO = new TradeRecordDO(pTrade->ExchangeID, pTrade->InstrumentID, pTrade->UserID, "");
		ret.reset(pDO);

		pDO->OrderID = ToUInt64(pTrade->OrderRef);
		pDO->OrderSysID = ToUInt64(pTrade->OrderSysID);
		pDO->Direction = pTrade->Direction == THOST_FTDC_D_Buy ? DirectionType::BUY : DirectionType::SELL;
		pDO->OpenClose = (OrderOpenCloseType)(pTrade->OffsetFlag - THOST_FTDC_OF_Open);
		pDO->Price = pTrade->Price;
		pDO->Volume = pTrade->Volume;
		pDO->TradeID = ToUInt64(pTrade->TradeID);
		pDO->TradeDate = pTrade->TradeDate;
		pDO->TradeTime = pTrade->TradeTime;
		pDO->TradingDay = std::atoi(pTrade->TradingDay);
		pDO->TradeType = (OrderTradingType)pTrade->TradeType;
		pDO->HedgeFlag = (HedgeType)(pTrade->HedgeFlag - THOST_FTDC_HF_Speculation);

		pDO->BrokerID = pTrade->BrokerID;
		pDO->InvestorID = pTrade->InvestorID;

		if (pDO->OpenClose == OrderOpenCloseType::CLOSETODAY && pDO->ExchangeID() != EXCHANGE_SHFE)
		{
			pDO->OpenClose = OrderOpenCloseType::CLOSE;
		}
	}

	return ret;
}

UserPositionExDO_Ptr XTUtility::ParseRawPosition(CThostFtdcInvestorPositionField * pRspPosition, const std::string& userId)
{
	std::string exchange;
	if (auto pInstrumentDO = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(pRspPosition->InstrumentID))
	{
		exchange = pInstrumentDO->ExchangeID();
	}

	std::string portfolio;
	//if (auto pPortfolioKey = PositionPortfolioMap::FindPortfolio(userId, pRspPosition->InstrumentID))
	//{
	//	portfolio = pPortfolioKey->PortfolioID();
	//}

	auto pDO = new UserPositionExDO(exchange, pRspPosition->InstrumentID, portfolio, userId);
	UserPositionExDO_Ptr ret(pDO);

	pDO->Direction = (PositionDirectionType)(pRspPosition->PosiDirection - THOST_FTDC_PD_Net);
	pDO->HedgeFlag = (HedgeType)(pRspPosition->HedgeFlag - THOST_FTDC_HF_Speculation);
	pDO->PositionDateFlag = (PositionDateFlagType)(pRspPosition->PositionDate - THOST_FTDC_PSD_Today);

	pDO->YdInitPosition = pRspPosition->YdPosition;

	if (pRspPosition->PositionDate == THOST_FTDC_PSD_Today)
	{
		// pDO->TdPosition = pRspPosition->Position;
		pDO->TdCost = pRspPosition->PositionCost;
		// pDO->TdProfit = pRspPosition->PositionProfit;
	}
	else
	{
		// pDO->YdPosition = pRspPosition->Position - pRspPosition->YdPosition;
		pDO->YdCost = pRspPosition->PositionCost;
		// pDO->YdProfit = pRspPosition->PositionProfit;
	}

	pDO->LongFrozenVolume = pRspPosition->LongFrozen;
	pDO->LongFrozenAmount = pRspPosition->LongFrozenAmount;
	pDO->ShortFrozenVolume = pRspPosition->ShortFrozen;
	pDO->ShortFrozenAmount = pRspPosition->ShortFrozenAmount;

	pDO->OpenVolume = pRspPosition->OpenVolume;
	pDO->CloseVolume = pRspPosition->CloseVolume;
	pDO->OpenAmount = pRspPosition->OpenAmount;
	pDO->CloseAmount = pRspPosition->CloseAmount;
	pDO->PreMargin = pRspPosition->PreMargin;
	pDO->UseMargin = pRspPosition->UseMargin;
	pDO->FrozenMargin = pRspPosition->FrozenMargin;
	pDO->FrozenCash = pRspPosition->FrozenCash;
	pDO->FrozenCommission = pRspPosition->FrozenCommission;
	pDO->CashIn = pRspPosition->CashIn;
	pDO->Commission = pRspPosition->Commission;
	pDO->CloseProfit = pRspPosition->CloseProfit;
	pDO->PreSettlementPrice = pRspPosition->PreSettlementPrice;
	pDO->SettlementPrice = pRspPosition->SettlementPrice;
	pDO->TradingDay = std::atoi(pRspPosition->TradingDay);
	pDO->SettlementID = pRspPosition->SettlementID;
	pDO->OpenCost = pRspPosition->OpenCost;
	pDO->ExchangeMargin = pRspPosition->ExchangeMargin;
	pDO->CombPosition = pRspPosition->CombPosition;
	pDO->CombLongFrozen = pRspPosition->CombLongFrozen;
	pDO->CombShortFrozen = pRspPosition->CombShortFrozen;
	/*pDO->CloseProfitByDate = pRspPosition->CloseProfitByDate;
	pDO->CloseProfitByTrade = pRspPosition->CloseProfitByTrade;
	pDO->MarginRateByMoney = pRspPosition->MarginRateByMoney;
	pDO->MarginRateByVolume = pRspPosition->MarginRateByVolume;*/


	return ret;
}

uint32_t XTUtility::ParseOrderMessageID(OrderStatusType orderStatus)
{
	uint32_t msgId = MSG_ID_UNDEFINED;
	switch (orderStatus)
	{
	case OrderStatusType::OPENING:
	case OrderStatusType::OPEN_REJECTED:
		msgId = MSG_ID_ORDER_NEW;
		break;
	case OrderStatusType::CANCELED:
	case OrderStatusType::CANCELING:
	case OrderStatusType::CANCEL_REJECTED:
		msgId = MSG_ID_ORDER_CANCEL;
		break;
	default:
		break;
	}

	return msgId;
}

std::string XTUtility::MakeUserID(const std::string& brokerID, const std::string& investorID)
{
	return brokerID + investorID;
}
