/***********************************************************************
 * Module:  CTPUtility.cpp
 * Author:  milk
 * Modified: 2015年7月16日 21:32:43
 * Purpose: Implementation of the class CTPUtility
 ***********************************************************************/

#include "CTPUtility.h"
#include "tradeapi/ThostFtdcUserApiStruct.h"
#include "../message/BizError.h"
#include "../utility/Encoding.h"
#include "../utility/commonconst.h"
#include "../common/BizErrorIDs.h"

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
		throw BizError(API_INNER_ERROR,
			std::move(Encoding::ToUTF8(pRsp->ErrorMsg, CHARSET_GB2312)),
			pRsp->ErrorID);
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPUtility::HasError(void* pRespInfo)
// Purpose:    Implementation of CTPUtility::HasError()
// Parameters:
// - pRespInfo
// Return:     bool
////////////////////////////////////////////////////////////////////////

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

BizError_Ptr CTPUtility::HasReturnError(const int rtnCode)
{
	switch (rtnCode)
	{
	case 0:
		return nullptr;
	case -1:
		return std::make_shared<BizError>(CONNECTION_ERROR, "Cannot connect to CTP server");
	case -2:
		return std::make_shared<BizError>(NO_PERMISSION, "Unresolved requests exeeding permission");
	case -3:
		return std::make_shared<BizError>(TOO_MANY_REQUEST, "The number of requests sent per second exeeding permission");
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
			ret = OrderStatus::OPENNING;
			break;
		case THOST_FTDC_OSS_InsertRejected:
			ret = OrderStatus::OPEN_REJECTED;
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

OrderDO_Ptr CTPUtility::ParseRawOrder(CThostFtdcOrderField *pOrder)
{
	auto pDO = new OrderDO(std::strtoull(pOrder->OrderRef, nullptr, 0),
		pOrder->ExchangeID, pOrder->InstrumentID, pOrder->UserID);
	OrderDO_Ptr ret(pDO);

	pDO->Direction = (pOrder->Direction == THOST_FTDC_D_Buy) ?
		DirectionType::BUY : DirectionType::SELL;
	pDO->OpenClose = (OrderOpenCloseType)(pOrder->CombOffsetFlag[0] - THOST_FTDC_OF_Open);
	pDO->LimitPrice = pOrder->LimitPrice;
	pDO->Volume = pOrder->VolumeTotalOriginal;
	pDO->StopPrice = pOrder->StopPrice;
	pDO->OrderSysID = std::strtoull(pOrder->OrderSysID, nullptr, 0);
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
	pDO->TradingDay = std::strtoul(pOrder->TradingDay, nullptr, 0);
	pDO->Message = std::move(Encoding::ToUTF8(pOrder->StatusMsg, CHARSET_GB2312));

	return ret;
}

OrderDO_Ptr CTPUtility::ParseRawOrderAction(
	CThostFtdcInputOrderActionField *pOrderAction,
	CThostFtdcRspInfoField *pRsp,
	OrderStatus orderstatus)
{
	auto pDO = new OrderDO(std::strtoull(pOrderAction->OrderRef, nullptr, 0),
		pOrderAction->ExchangeID, pOrderAction->InstrumentID, pOrderAction->UserID);
	OrderDO_Ptr ret(pDO);
	pDO->OrderSysID = std::strtoull(pOrderAction->OrderSysID, nullptr, 0);
	pDO->OrderStatus = orderstatus;
	pDO->LimitPrice = pOrderAction->LimitPrice;
	pDO->Active = false;

	if (pRsp) {
		pDO->ErrorCode = pRsp->ErrorID;
		pDO->Message = std::move(Encoding::ToUTF8(pRsp->ErrorMsg, CHARSET_GB2312));
	}

	return ret;
}

OrderDO_Ptr CTPUtility::ParseRawOrderInput(
	CThostFtdcInputOrderField *pOrderInput,
	CThostFtdcRspInfoField *pRsp,
	OrderStatus orderstatus)
{
	auto pDO = new OrderDO(std::strtoull(pOrderInput->OrderRef, nullptr, 0),
		"", pOrderInput->InstrumentID, pOrderInput->UserID);
	OrderDO_Ptr ret(pDO);

	pDO->Direction = (pOrderInput->Direction == THOST_FTDC_D_Buy) ? DirectionType::BUY : DirectionType::SELL;
	pDO->LimitPrice = pOrderInput->LimitPrice;
	pDO->Volume = pOrderInput->VolumeTotalOriginal;
	pDO->StopPrice = pOrderInput->StopPrice;
	pDO->OrderStatus = orderstatus;
	pDO->Active = false;

	if (pRsp) {
		pDO->ErrorCode = pRsp->ErrorID;
		pDO->Message = std::move(Encoding::ToUTF8(pRsp->ErrorMsg, CHARSET_GB2312));
	}

	return ret;
}