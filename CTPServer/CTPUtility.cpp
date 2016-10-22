/***********************************************************************
 * Module:  CTPUtility.cpp
 * Author:  milk
 * Modified: 2015年7月16日 21:32:43
 * Purpose: Implementation of the class CTPUtility
 ***********************************************************************/

#include "CTPUtility.h"
#include "tradeapi/ThostFtdcUserApiStruct.h"
#include "../message/BizError.h"
#include <boost/locale/encoding.hpp>
#include "../utility/commonconst.h"
#include "../common/BizErrorIDs.h"
#include "../bizutility/ContractCache.h"
#include "../utility/stringutility.h"
#include "../ordermanager/OrderSeqGen.h"

#include "../message/DefMessageID.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPUtility::CheckError()
 // Purpose:    Implementation of CTPUtility::CheckError()
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

void CTPUtility::CheckError(const void* pRspInfo)
{
	if (HasError(pRspInfo))
	{
		auto pRsp = (CThostFtdcRspInfoField*)pRspInfo;
		throw ApiException(pRsp->ErrorID,
			std::move(boost::locale::conv::to_utf<char>(pRsp->ErrorMsg, CHARSET_GB2312)));
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPUtility::HasError(void* pRespInfo)
// Purpose:    Implementation of CTPUtility::HasError()
// Parameters:
// - pRespInfo
// Return:     bool
////////////////////////////////////////////////////////////////////////

void CTPUtility::CheckNotFound(const void * pRspData)
{
	if (!pRspData)
	{
		throw NotFoundException();
	}
}

bool CTPUtility::HasError(const void* pRspInfo)
{
	bool ret = false;

	if (pRspInfo)
	{
		auto pRsp = (CThostFtdcRspInfoField*)pRspInfo;
		if (pRsp->ErrorID)
		{
			ret = true;
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPUtility::CheckReturnError(int rtncode)
// Purpose:    Implementation of CTPUtility::CheckReturnError()
// Parameters:
// - rtncode
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPUtility::CheckReturnError(const int rtnCode)
{
	if (auto bizError_Ptr = HasReturnError(rtnCode))
		throw *bizError_Ptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPUtility::HasReturnError(int rtnCode, BizError& bizError)
// Purpose:    Implementation of CTPUtility::HasReturnError()
// Parameters:
// - rtnCode
// - bizError
// Return:     bool
////////////////////////////////////////////////////////////////////////

std::shared_ptr<ApiException> CTPUtility::HasReturnError(const int rtnCode)
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
// Name:       CTPUtility::HasReturnError(int rtnCode, BizError& bizError)
// Purpose:    Implementation of CTPUtility::HasReturnError()
// Parameters:
// - rtnCode
// - bizError
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool CTPUtility::IsOrderActive(const int status)
{
	bool ret = true;

	switch (status)
	{
	case THOST_FTDC_OST_AllTraded:
	case THOST_FTDC_OST_PartTradedNotQueueing:
	case THOST_FTDC_OST_NoTradeNotQueueing:
	case THOST_FTDC_OST_Canceled:
	case THOST_FTDC_OST_Unknown:
		ret = false;
		break;
	default:
		break;
	}

	return ret;
}


OrderStatus CTPUtility::CheckOrderStatus(const int status, const int submitStatus)
{
	OrderStatus ret = OrderStatus::UNDEFINED;

	switch (status)
	{
	case THOST_FTDC_OST_Canceled:
		ret = OrderStatus::CANCELED;
		break;
	case THOST_FTDC_OST_AllTraded:
		ret = OrderStatus::ALL_TRADED;
		break;
	case THOST_FTDC_OST_NoTradeQueueing:
		ret = OrderStatus::OPENED;
		break;
	case THOST_FTDC_OST_NoTradeNotQueueing:
		ret = OrderStatus::OPEN_REJECTED;
		break;
	case THOST_FTDC_OST_PartTradedQueueing:
		ret = OrderStatus::PARTIAL_TRADING;
		break;
	case THOST_FTDC_OST_PartTradedNotQueueing:
		ret = OrderStatus::PARTIAL_TRADED;
		break;
	default:
		switch (submitStatus)
		{
		case THOST_FTDC_OSS_InsertSubmitted:
			ret = OrderStatus::SUBMITTING;
			break;
		case THOST_FTDC_OSS_Accepted:
			ret = OrderStatus::OPENED;
			break;
		case THOST_FTDC_OSS_InsertRejected:
			ret = OrderStatus::OPEN_REJECTED;
			break;
		case THOST_FTDC_OSS_CancelSubmitted:
			ret = OrderStatus::CANCELING;
			break;
		case THOST_FTDC_OSS_CancelRejected:
			ret = OrderStatus::CANCEL_REJECTED;
			break;
		default:
			break;
		}
	}

	return ret;
}

OrderDO_Ptr CTPUtility::ParseRawOrder(CThostFtdcOrderField *pOrder, OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
		baseOrder.reset(new OrderDO(ToUInt64(pOrder->OrderRef),
			pOrder->ExchangeID, pOrder->InstrumentID, pOrder->UserID));

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
	pDO->VolumeRemain = pOrder->VolumeTotal;
	pDO->TIF = pOrder->TimeCondition == THOST_FTDC_TC_IOC ?
		OrderTIFType::IOC : OrderTIFType::GFD;
	char timebuf[20];
	sprintf(timebuf, "%s %s", pOrder->InsertDate, pOrder->InsertTime);
	pDO->InsertTime = timebuf;
	pDO->UpdateTime = pOrder->UpdateTime;
	pDO->CancelTime = pOrder->CancelTime;
	pDO->TradingDay = pOrder->TradingDay;
	pDO->Message = std::move(boost::locale::conv::to_utf<char>(pOrder->StatusMsg, CHARSET_GB2312));
	pDO->SessionID = pOrder->SessionID;

	return baseOrder;
}

OrderDO_Ptr CTPUtility::ParseRawOrder(
	CThostFtdcInputOrderActionField *pOrderAction,
	CThostFtdcRspInfoField *pRsp,
	OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
		baseOrder.reset(new OrderDO(ToUInt64(pOrderAction->OrderRef),
			pOrderAction->ExchangeID, pOrderAction->InstrumentID, pOrderAction->UserID));

	auto pDO = baseOrder.get();
	pDO->OrderSysID = ToUInt64(pOrderAction->OrderSysID);
	pDO->LimitPrice = pOrderAction->LimitPrice;
	pDO->Active = pRsp == nullptr;
	pDO->OrderStatus = pRsp ? OrderStatus::CANCEL_REJECTED : OrderStatus::CANCELING;
	pDO->SessionID = pOrderAction->SessionID;

	if (pRsp)
	{
		pDO->ErrorCode = pRsp->ErrorID;
		pDO->Message = std::move(boost::locale::conv::to_utf<char>(pRsp->ErrorMsg, CHARSET_GB2312));
	}

	return baseOrder;
}

OrderDO_Ptr CTPUtility::ParseRawOrder(
	CThostFtdcInputOrderField *pOrderInput,
	CThostFtdcRspInfoField *pRsp,
	int sessionID,
	OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
	{
		const char* pExchange = "";
		if (auto pInstument = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(pOrderInput->InstrumentID))
			pExchange = pInstument->ExchangeID().data();

		baseOrder.reset(new OrderDO(ToUInt64(pOrderInput->OrderRef),
			pExchange, pOrderInput->InstrumentID, pOrderInput->UserID));
	}

	auto pDO = baseOrder.get();
	pDO->SessionID = sessionID;
	pDO->Direction = (pOrderInput->Direction == THOST_FTDC_D_Buy) ? DirectionType::BUY : DirectionType::SELL;
	pDO->LimitPrice = pOrderInput->LimitPrice;
	pDO->Volume = pOrderInput->VolumeTotalOriginal;
	pDO->VolumeRemain = pOrderInput->VolumeTotalOriginal;
	pDO->StopPrice = pOrderInput->StopPrice;
	pDO->Active = pRsp == nullptr;
	pDO->OrderStatus = pRsp ? OrderStatus::OPEN_REJECTED : OrderStatus::SUBMITTING;

	auto now = std::time(nullptr);
	auto tm = std::localtime(&now);
	char timebuf[20];
	std::strftime(timebuf, sizeof(timebuf), "%Y%m%d %T", tm);
	pDO->InsertTime = timebuf;

	if (pRsp) {
		pDO->ErrorCode = pRsp->ErrorID;
		pDO->Message = std::move(boost::locale::conv::to_utf<char>(pRsp->ErrorMsg, CHARSET_GB2312));
	}

	return baseOrder;
}

OrderDO_Ptr CTPUtility::ParseRawOrder(CThostFtdcOrderActionField * pOrderAction, CThostFtdcRspInfoField * pRsp, OrderDO_Ptr baseOrder)
{
	if (!baseOrder)
		baseOrder.reset(new OrderDO(ToUInt64(pOrderAction->OrderRef),
			pOrderAction->ExchangeID, pOrderAction->InstrumentID, pOrderAction->UserID));

	auto pDO = baseOrder.get();
	pDO->OrderSysID = ToUInt64(pOrderAction->OrderSysID);
	pDO->LimitPrice = pOrderAction->LimitPrice;
	pDO->Active = pRsp == nullptr;
	pDO->OrderStatus = pRsp ? OrderStatus::CANCEL_REJECTED : OrderStatus::CANCELING;
	pDO->SessionID = pOrderAction->SessionID;

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

TradeRecordDO_Ptr CTPUtility::ParseRawTrade(CThostFtdcTradeField * pTrade)
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
		pDO->TradingDay = pTrade->TradingDay;
		pDO->TradeType = (TradingType)pTrade->TradeType;
		pDO->HedgeFlag = (HedgeType)(pTrade->HedgeFlag - THOST_FTDC_HF_Speculation);
	}

	return ret;
}

BankOpResultDO_Ptr CTPUtility::ParseRawTransfer(CThostFtdcReqTransferField * pReqTransfer, CThostFtdcRspInfoField *pRsp)
{
	BankOpResultDO_Ptr ret;

	if (pReqTransfer)
	{
		auto pDO = new BankOpResultDO;
		ret.reset(pDO);

		pDO->AccountID = pReqTransfer->AccountID;
		pDO->Password = pReqTransfer->Password;
		pDO->BankAccount = pReqTransfer->BankAccount;
		pDO->BankAccType = (BankAccountType)(pReqTransfer->BankAccType - THOST_FTDC_FAT_BankBook + 1);
		pDO->BankPassword = pReqTransfer->BankPassWord;
		pDO->BankBranchID = pReqTransfer->BankBranchID;
		pDO->BankID = pReqTransfer->BankID;
		pDO->BankSerial = pReqTransfer->BankSerial;
		pDO->BrokerBranchID = pReqTransfer->BrokerBranchID;
		pDO->BrokerFee = pReqTransfer->BrokerFee;
		pDO->BrokerID = pReqTransfer->BrokerID;
		pDO->CurrencyID = pReqTransfer->CurrencyID;
		pDO->CustFee = pReqTransfer->CustFee;
		pDO->FutureSerial = std::to_string(pReqTransfer->FutureSerial);
		pDO->SerialNum = std::to_string(pReqTransfer->PlateSerial);
		pDO->TradeAmount = pReqTransfer->TradeAmount;
		pDO->TradingDay = pReqTransfer->TradingDay;
		pDO->TradeDate = pReqTransfer->TradeDate;
		pDO->TradeTime = pReqTransfer->TradeTime;
		pDO->TradeCode = pReqTransfer->TradeCode;

		if (!pRsp)
		{
			pDO->ErrorID = pRsp->ErrorID;
			if (!pRsp->ErrorMsg[0])
				pDO->ErrorMsg = std::move(boost::locale::conv::to_utf<char>(pRsp->ErrorMsg, CHARSET_GB2312));
		}
	}

	return ret;
}

BankOpResultDO_Ptr CTPUtility::ParseRawTransfer(CThostFtdcRspTransferField * pRspTransfer, CThostFtdcRspInfoField *pRsp)
{
	BankOpResultDO_Ptr ret;

	if (pRspTransfer)
	{
		auto pDO = new BankOpResultDO;
		ret.reset(pDO);

		pDO->AccountID = pRspTransfer->AccountID;
		pDO->Password = pRspTransfer->Password;
		pDO->BankAccount = pRspTransfer->BankAccount;
		pDO->BankAccType = (BankAccountType)(pRspTransfer->BankAccType - THOST_FTDC_FAT_BankBook + 1);
		pDO->BankPassword = pRspTransfer->BankPassWord;
		pDO->BankBranchID = pRspTransfer->BankBranchID;
		pDO->BankID = pRspTransfer->BankID;
		pDO->BankSerial = pRspTransfer->BankSerial;
		pDO->BrokerBranchID = pRspTransfer->BrokerBranchID;
		pDO->BrokerFee = pRspTransfer->BrokerFee;
		pDO->BrokerID = pRspTransfer->BrokerID;
		pDO->CurrencyID = pRspTransfer->CurrencyID;
		pDO->CustFee = pRspTransfer->CustFee;
		pDO->FutureSerial = std::to_string(pRspTransfer->FutureSerial);
		pDO->SerialNum = std::to_string(pRspTransfer->PlateSerial);
		pDO->TradeAmount = pRspTransfer->TradeAmount;
		pDO->TradingDay = pRspTransfer->TradingDay;
		pDO->TradeDate = pRspTransfer->TradeDate;
		pDO->TradeTime = pRspTransfer->TradeTime;
		pDO->TradeCode = pRspTransfer->TradeCode;

		pDO->ErrorID = pRspTransfer->ErrorID;
		if (pRspTransfer->ErrorMsg[0])
			pDO->ErrorMsg = std::move(boost::locale::conv::to_utf<char>(pRspTransfer->ErrorMsg, CHARSET_GB2312));
	}

	return ret;
}

BankOpResultDO_Ptr CTPUtility::ParseRawTransfer(CThostFtdcTransferSerialField * pRspTransfer)
{
	BankOpResultDO_Ptr ret;

	if (pRspTransfer)
	{
		auto pDO = new BankOpResultDO;
		ret.reset(pDO);

		pDO->AccountID = pRspTransfer->AccountID;
		pDO->BankAccount = pRspTransfer->BankAccount;
		pDO->BankAccType = (BankAccountType)(pRspTransfer->BankAccType - THOST_FTDC_FAT_BankBook + 1);
		pDO->BankBranchID = pRspTransfer->BankBranchID;
		pDO->BankID = pRspTransfer->BankID;
		pDO->BankSerial = pRspTransfer->BankSerial;
		pDO->BrokerBranchID = pRspTransfer->BrokerBranchID;
		pDO->BrokerFee = pRspTransfer->BrokerFee;
		pDO->BrokerID = pRspTransfer->BrokerID;
		pDO->CurrencyID = pRspTransfer->CurrencyID;
		pDO->CustFee = pRspTransfer->CustFee;
		pDO->FutureAccType = (BankAccountType)(pRspTransfer->FutureAccType - THOST_FTDC_FAT_BankBook + 1);
		pDO->FutureSerial = std::to_string(pRspTransfer->FutureSerial);
		pDO->SerialNum = std::to_string(pRspTransfer->PlateSerial);
		pDO->TradeAmount = pRspTransfer->TradeAmount;
		pDO->TradingDay = pRspTransfer->TradingDay;
		pDO->TradeDate = pRspTransfer->TradeDate;
		pDO->TradeTime = pRspTransfer->TradeTime;
		pDO->TradeCode = pRspTransfer->TradeCode;

		pDO->ErrorID = pRspTransfer->ErrorID;
		if (pRspTransfer->ErrorMsg[0])
			pDO->ErrorMsg = std::move(boost::locale::conv::to_utf<char>(pRspTransfer->ErrorMsg, CHARSET_GB2312));
	}

	return ret;
}

UserAccountRegisterDO_Ptr CTPUtility::ParseUserBankAccout(CThostFtdcAccountregisterField * pAccount, CThostFtdcRspInfoField * pRsp)
{
	UserAccountRegisterDO_Ptr ret;

	if (pAccount)
	{
		auto pDO = new UserAccountRegisterDO;
		ret.reset(pDO);
		pDO->AccountID = pAccount->AccountID;
		pDO->BankAccount = pAccount->BankAccount;
		pDO->BankAccType = (BankAccountType)(pAccount->BankAccType - THOST_FTDC_BAT_BankBook + 1);
		pDO->BankBranchID = pAccount->BankBranchID;
		pDO->BankID = pAccount->BankID;
		pDO->BrokerBranchID = pAccount->BrokerBranchID;
		pDO->BrokerID = pAccount->BrokerID;
		pDO->CurrencyID = pAccount->CurrencyID;
		if (pAccount->CustType >= THOST_FTDC_CUSTT_Person)
			pDO->CustType = (CustomerType)(pAccount->CustType - THOST_FTDC_CUSTT_Person);
		pDO->CustomerName = boost::locale::conv::to_utf<char>(pAccount->CustomerName, CHARSET_GB2312);
		pDO->IdentifiedCardNo = pAccount->IdentifiedCardNo;
	}

	return ret;
}

UserAccountRegisterDO_Ptr CTPUtility::ParseUserBankAccout(CThostFtdcReqQueryAccountField * pAccount, CThostFtdcRspInfoField * pRsp)
{
	UserAccountRegisterDO_Ptr ret;

	if (pAccount)
	{
		auto pDO = new UserAccountRegisterDO;
		ret.reset(pDO);
		pDO->AccountID = pAccount->AccountID;
		pDO->BankAccount = pAccount->BankAccount;
		pDO->BankAccType = (BankAccountType)(pAccount->BankAccType - THOST_FTDC_BAT_BankBook + 1);
		pDO->BankBranchID = pAccount->BankBranchID;
		pDO->BankID = pAccount->BankID;
		pDO->BrokerBranchID = pAccount->BrokerBranchID;
		pDO->BrokerID = pAccount->BrokerID;
		pDO->CurrencyID = pAccount->CurrencyID;
		if (pAccount->CustType >= THOST_FTDC_CUSTT_Person)
			pDO->CustType = (CustomerType)(pAccount->CustType - THOST_FTDC_CUSTT_Person);
		pDO->CustomerName = boost::locale::conv::to_utf<char>(pAccount->CustomerName, CHARSET_GB2312);
		pDO->IdentifiedCardNo = pAccount->IdentifiedCardNo;
	}

	return ret;
}

UserAccountRegisterDO_Ptr CTPUtility::ParseUserBankAccout(CThostFtdcNotifyQueryAccountField * pAccount)
{
	UserAccountRegisterDO_Ptr ret;

	if (pAccount)
	{
		auto pDO = new UserAccountRegisterDO;
		ret.reset(pDO);
		pDO->AccountID = pAccount->AccountID;
		pDO->BankAccount = pAccount->BankAccount;
		pDO->BankAccType = (BankAccountType)(pAccount->BankAccType - THOST_FTDC_BAT_BankBook + 1);
		pDO->BankBranchID = pAccount->BankBranchID;
		pDO->BankID = pAccount->BankID;
		pDO->BrokerBranchID = pAccount->BrokerBranchID;
		pDO->BrokerID = pAccount->BrokerID;
		pDO->CurrencyID = pAccount->CurrencyID;
		if (pAccount->CustType >= THOST_FTDC_CUSTT_Person)
			pDO->CustType = (CustomerType)(pAccount->CustType - THOST_FTDC_CUSTT_Person);
		pDO->CustomerName = boost::locale::conv::to_utf<char>(pAccount->CustomerName, CHARSET_GB2312);
		pDO->IdentifiedCardNo = pAccount->IdentifiedCardNo;
		pDO->BankFetchAmount = pAccount->BankFetchAmount;
		pDO->BankUseAmount = pAccount->BankUseAmount;
	}

	return ret;
}

uint32_t CTPUtility::ParseMessageID(OrderStatus status)
{
	uint32_t msgId = MSG_ID_ORDER_UPDATE;
	switch (status)
	{
	case OrderStatus::CANCELING:
	case OrderStatus::CANCEL_REJECTED:
		msgId = MSG_ID_ORDER_CANCEL;
		break;
	case OrderStatus::SUBMITTING:
	case OrderStatus::OPEN_REJECTED:
		msgId = MSG_ID_ORDER_NEW;
		break;
	default:
		break;
	}

	return msgId;
}
