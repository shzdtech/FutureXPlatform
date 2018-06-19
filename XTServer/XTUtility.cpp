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
#include "../ordermanager/OrderReqCache.h"
#include "../ordermanager/OrderSeqGen.h"

#include "../message/DefMessageID.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       XTUtility::CheckError()
 // Purpose:    Implementation of XTUtility::CheckError()
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

void XTUtility::CheckError(const void* pRspInfo)
{
	auto pRsp = (XtError*)pRspInfo;
	if (HasError(pRsp))
	{	
		throw ApiException(pRsp->errorID(),
			std::move(boost::locale::conv::to_utf<char>(pRsp->errorMsg(), CHARSET_GB2312)));
	}
}

void XTUtility::CheckError(const char * pErrMsg)
{
	if (!pErrMsg && pErrMsg[0])
	{
		throw ApiException(std::move(boost::locale::conv::to_utf<char>(pErrMsg, CHARSET_GB2312)));
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
		throw UserException("Not login to XT trade server ...");
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

bool XTUtility::HasError(const XtError* pRspInfo)
{
	bool ret = false;

	if (pRspInfo)
	{
		ret = pRspInfo->isSuccess();
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

AccountInfoDO_Ptr XTUtility::ParseRawAccountInfo(const CAccountDetail * pAccountInfo)
{
	AccountInfoDO_Ptr ret;
	auto pDO = new AccountInfoDO;
	ret.reset(pDO);

	pDO->AccountID = pAccountInfo->m_strAccountID;
	pDO->PreMortgage = pAccountInfo->m_dMortgage;
	pDO->PreCredit = pAccountInfo->m_dCredit;
	pDO->PreDeposit = pAccountInfo->PreDeposit;
	pDO->PreBalance = pAccountInfo->PreBalance;
	pDO->PreMargin = pAccountInfo->PreMargin;
	pDO->InterestBase = pAccountInfo->InterestBase;
	pDO->Interest = pAccountInfo->Interest;
	pDO->Deposit = pAccountInfo->m_dDeposit;
	pDO->Withdraw = pAccountInfo->m_dWithdraw;
	pDO->FrozenMargin = pAccountInfo->FrozenMargin;
	pDO->FrozenCash = pAccountInfo->FrozenCash;
	pDO->FrozenCommission = pAccountInfo->FrozenCommission;
	pDO->CurrMargin = pAccountInfo->m_dCurrMargin;
	pDO->CashIn = pAccountInfo->m_dCashIn;
	pDO->Commission = pAccountInfo->m_dCommission;
	pDO->CloseProfit = pAccountInfo->m_dCloseProfit;
	pDO->PositionProfit = pAccountInfo->m_dPositionProfit;
	pDO->Balance = pAccountInfo->m_dBalance;
	pDO->Available = pAccountInfo->m_dAvailable;
	pDO->WithdrawQuota = pAccountInfo->m_dWithdraw;
	pDO->Reserve = pAccountInfo->Reserve;
	pDO->TradingDay = std::atoi(pAccountInfo->TradingDay);
	pDO->SettlementID = pAccountInfo->SettlementID;
	pDO->Credit = pAccountInfo->Credit;
	pDO->Mortgage = pAccountInfo->m_dMortgage;
	pDO->ExchangeMargin = pAccountInfo->ExchangeMargin;
	pDO->DeliveryMargin = pAccountInfo->DeliveryMargin;
	pDO->ExchangeDeliveryMargin = pAccountInfo->ExchangeDeliveryMargin;
	pDO->ReserveBalance = pAccountInfo->Reserve;
	pDO->RiskRatio = pDO->Balance > 0 ? pDO->CurrMargin / pDO->Balance : 0;

	return ret;
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
		return std::make_shared<ApiException>(CONNECTION_ERROR, "Cannot connect to XT server");
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

EOperationType XTUtility::GetOperationType(ProductType productType, DirectionType direction, OrderOpenCloseType openClose)
{
	EOperationType ret;
	return ret;
}

bool XTUtility::IsOrderActive(EEntrustStatus status)
{
	bool ret = true;

	switch (status)
	{
	case ENTRUST_STATUS_PART_CANCEL:
	case ENTRUST_STATUS_CANCELED:
	case ENTRUST_STATUS_PART_SUCC:
	case ENTRUST_STATUS_SUCCEEDED:
	case ENTRUST_STATUS_JUNK:
		ret = false;
		break;
	default:
		break;
	}

	return ret;
}


OrderStatusType XTUtility::CheckOrderStatus(EEntrustStatus status, EEntrustSubmitStatus submitStatus)
{
	OrderStatusType ret = OrderStatusType::UNDEFINED;

	switch (status)
	{
	case ENTRUST_STATUS_CANCELED:
		ret = OrderStatusType::CANCELED;
		break;
	case ENTRUST_STATUS_SUCCEEDED:
		ret = OrderStatusType::ALL_TRADED;
		break;
	case ENTRUST_STATUS_REPORTED:
		ret = OrderStatusType::OPENED;
		break;
	case ENTRUST_STATUS_JUNK:
		ret = OrderStatusType::OPEN_REJECTED;
		break;
	case ENTRUST_STATUS_PARTSUCC_CANCEL:
		ret = OrderStatusType::PARTIAL_TRADING;
		break;
	case ENTRUST_STATUS_PART_SUCC:
		ret = OrderStatusType::PARTIAL_TRADED;
		break;
	default:
		switch (submitStatus)
		{
		case ENTRUST_SUBMIT_STATUS_InsertSubmitted:
			ret = OrderStatusType::OPENING;
			break;
		case ENTRUST_SUBMIT_STATUS_InsertRejected:
			ret = OrderStatusType::OPEN_REJECTED;
			break;
		case ENTRUST_SUBMIT_STATUS_CancelSubmitted:
			ret = OrderStatusType::CANCELING;
			break;
		case ENTRUST_SUBMIT_STATUS_CancelRejected:
			ret = OrderStatusType::CANCEL_REJECTED;
			break;
		default:
			break;
		}
	}

	return ret;
}

OrderDO_Ptr XTUtility::ParseRawOrder(int requestId, int orderId, const XtError *pOrderError)
{
	OrderDO_Ptr ret;

	OrderRequestDO orderReqDO;
	if (OrderReqCache::Find(requestId, orderReqDO))
	{
		orderReqDO.OrderID = orderId;
		OrderReqCache::Insert(OrderSeqGen::GetOrderID(orderId), orderReqDO);
		OrderReqCache::Remove(requestId);
		ret = std::make_shared<OrderDO>(orderReqDO);
		ret->Active = false;
		ret->OrderStatus = OrderStatusType::OPEN_REJECTED;
		if (HasError(pOrderError))
		{
			ret->ErrorCode = pOrderError->errorID();
			ret->Message = std::move(boost::locale::conv::to_utf<char>(pOrderError->errorMsg(), CHARSET_GB2312));
		}
	}

	return ret;
}

OrderDO_Ptr XTUtility::ParseRawOrder(const COrderError * pOrderError)
{
	OrderDO_Ptr ret;

	OrderRequestDO orderReqDO;
	if (OrderReqCache::Find(pOrderError->m_nRequestID, orderReqDO))
	{
		orderReqDO.OrderID = pOrderError->m_nOrderID;
		OrderReqCache::Remove(pOrderError->m_nRequestID);
		ret = std::make_shared<OrderDO>(orderReqDO);
		ret->Active = false;
		ret->OrderStatus = OrderStatusType::OPEN_REJECTED;
		ret->ErrorCode = pOrderError->m_nErrorID;
		ret->Message = std::move(boost::locale::conv::to_utf<char>(pOrderError->m_strErrorMsg, CHARSET_GB2312));
	}
	return ret;
}

OrderDO_Ptr XTUtility::ParseRawOrder(const CCancelError * pOrderError)
{
	OrderDO_Ptr ret;

	OrderRequestDO orderReqDO;
	if (OrderReqCache::Find(pOrderError->m_nRequestID, orderReqDO))
	{
		orderReqDO.OrderID = pOrderError->m_nOrderID;
		ret = std::make_shared<OrderDO>(orderReqDO);
		ret->Active = false;
		ret->OrderStatus = OrderStatusType::CANCEL_REJECTED;
		ret->ErrorCode = pOrderError->m_nErrorID;
		ret->Message = std::move(boost::locale::conv::to_utf<char>(pOrderError->m_strErrorMsg, CHARSET_GB2312));
	}
	return ret;
}

OrderDO_Ptr XTUtility::ParseRawOrder(const COrderDetail *pOrder, int sessionID, OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
	{
		baseOrder.reset(new OrderDO(pOrder->m_nOrderID, pOrder->m_strExchangeID, pOrder->m_strInstrumentID, pOrder->m_strAccountID));
	}

	auto pDO = baseOrder.get();
	pDO->SessionID = sessionID;
	pDO->Direction = (pOrder->m_nDirection == ENTRUST_BUY) ? DirectionType::BUY : DirectionType::SELL;
	pDO->OpenClose = (OrderOpenCloseType)(pOrder->CombOffsetFlag[0] - THOST_FTDC_OF_Open);
	pDO->LimitPrice = pOrder->m_dLimitPrice;
	pDO->Volume = pOrder->m_nTotalVolume;
	pDO->StopPrice = pOrder->m_dAveragePrice;
	pDO->OrderSysID = ToUInt64(pOrder->m_strOrderSysID);
	pDO->Active = IsOrderActive(pOrder->m_eOrderStatus);
	pDO->OrderStatus = CheckOrderStatus(pOrder->m_eOrderStatus, pOrder->m_eOrderSubmitStatus);
	pDO->VolumeTraded = pOrder->m_nTradedVolume;
	pDO->TIF = OrderTIFType::GFD;
	pDO->VolCondition = (OrderVolType)(pOrder->VolumeCondition - THOST_FTDC_VC_AV);

	pDO->InsertDate = pOrder->m_strInsertDate;
	pDO->InsertTime = pOrder->m_strInsertTime;

	pDO->Message = std::move(boost::locale::conv::to_utf<char>(pOrder->m_strErrorMsg, CHARSET_GB2312));

	return baseOrder;
}

void XTUtility::ParseRawOrder(OrderDO_Ptr& baseOrder, const XtError * pRsp, int sessionID)
{
	baseOrder->SessionID = sessionID;
	if(HasError(pRsp))
		baseOrder->Message = std::move(boost::locale::conv::to_utf<char>(pRsp->errorMsg(), CHARSET_GB2312));
}

TradeRecordDO_Ptr XTUtility::ParseRawTrade(const CDealDetail * pTrade)
{
	TradeRecordDO_Ptr ret;
	if (pTrade)
	{
		auto pDO = new TradeRecordDO(pTrade->m_strExchangeID, pTrade->m_strInstrumentID, pTrade->UserID, "");
		ret.reset(pDO);

		pDO->OrderID = pTrade->m_nOrderID;
		pDO->OrderSysID = ToUInt64(pTrade->m_strOrderSysID);
		pDO->Direction = (pTrade->m_nDirection == ENTRUST_BUY) ? DirectionType::BUY : DirectionType::SELL;
		pDO->OpenClose = (OrderOpenCloseType)(pTrade->OffsetFlag - THOST_FTDC_OF_Open);
		pDO->Price = pTrade->m_dAveragePrice;
		pDO->Volume = pTrade->m_nVolume;
		pDO->TradeID = ToUInt64(pTrade->m_strTradeID);
		pDO->TradeDate = pTrade->m_strTradeDate;
		pDO->TradeTime = pTrade->m_strTradeTime;
		pDO->TradingDay = std::atoi(pTrade->TradingDay);
		pDO->TradeType = (OrderTradingType)pTrade->TradeType;
		pDO->HedgeFlag = (HedgeType)(pTrade->HedgeFlag - THOST_FTDC_HF_Speculation);

		pDO->BrokerID = pTrade->BrokerID;
		pDO->InvestorID = pTrade->m_strAccountID;

		if (pDO->OpenClose == OrderOpenCloseType::CLOSETODAY && pDO->ExchangeID() != EXCHANGE_SHFE)
		{
			pDO->OpenClose = OrderOpenCloseType::CLOSE;
		}
	}

	return ret;
}

UserPositionExDO_Ptr XTUtility::ParseRawPosition(const CPositionDetail * pRspPosition, const std::string& userId)
{
	std::string portfolio;
	//if (auto pPortfolioKey = PositionPortfolioMap::FindPortfolio(userId, pRspPosition->InstrumentID))
	//{
	//	portfolio = pPortfolioKey->PortfolioID();
	//}

	auto pDO = new UserPositionExDO(pRspPosition->m_strExchangeID, pRspPosition->m_strInstrumentID, portfolio, userId);
	UserPositionExDO_Ptr ret(pDO);

	pDO->Direction = (PositionDirectionType)(pRspPosition->PosiDirection - THOST_FTDC_PD_Net);
	pDO->HedgeFlag = (HedgeType)(pRspPosition->HedgeFlag - THOST_FTDC_HF_Speculation);
	pDO->PositionDateFlag = (PositionDateFlagType)(pRspPosition->PositionDate - THOST_FTDC_PSD_Today);

	pDO->YdInitPosition = pRspPosition->m_nYesterdayVolume;

	if (pRspPosition->m_bIsToday)
	{
		// pDO->TdPosition = pRspPosition->Position;
		pDO->TdCost = pRspPosition->m_dPositionCost;
		// pDO->TdProfit = pRspPosition->PositionProfit;
	}
	else
	{
		// pDO->YdPosition = pRspPosition->Position - pRspPosition->YdPosition;
		pDO->YdCost = pRspPosition->m_dPositionCost;
		// pDO->YdProfit = pRspPosition->PositionProfit;
	}

	pDO->LongFrozenVolume = pRspPosition->LongFrozen;
	pDO->LongFrozenAmount = pRspPosition->LongFrozenAmount;
	pDO->ShortFrozenVolume = pRspPosition->ShortFrozen;
	pDO->ShortFrozenAmount = pRspPosition->ShortFrozenAmount;

	pDO->OpenVolume = pRspPosition->OpenVolume;
	pDO->CloseVolume = pRspPosition->m_nCloseVolume;
	pDO->OpenAmount = pRspPosition->OpenAmount;
	pDO->CloseAmount = pRspPosition->m_dCloseAmount;
	pDO->PreMargin = pRspPosition->PreMargin;
	pDO->UseMargin = pRspPosition->m_dMargin;
	pDO->FrozenMargin = pRspPosition->FrozenMargin;
	pDO->FrozenCash = pRspPosition->FrozenCash;
	pDO->FrozenCommission = pRspPosition->FrozenCommission;
	pDO->CashIn = pRspPosition->CashIn;
	pDO->Commission = pRspPosition->Commission;
	pDO->CloseProfit = pRspPosition->m_dCloseProfit;
	pDO->PreSettlementPrice = pRspPosition->m_dLastSettlementPrice;
	pDO->SettlementPrice = pRspPosition->m_dSettlementPrice;
	pDO->TradingDay = std::atoi(pRspPosition->TradingDay);
	pDO->SettlementID = pRspPosition->SettlementID;
	pDO->OpenCost = pRspPosition->m_dOpenCost;
	pDO->ExchangeMargin = pRspPosition->ExchangeMargin;
	pDO->CombPosition = pRspPosition->CombPosition;
	pDO->CombLongFrozen = pRspPosition->CombLongFrozen;
	pDO->CombShortFrozen = pRspPosition->CombShortFrozen;
	pDO->LastPrice = pRspPosition->m_dLastPrice;
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
