/***********************************************************************
 * Module:  CTPCancleOrder.cpp
 * Author:  milk
 * Modified: 2015年10月26日 11:31:17
 * Purpose: Implementation of the class CTPCancleOrder
 ***********************************************************************/

#include "CTPCancelOrder.h"
#include "CTPRawAPI.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"

#include "../dataobject/OrderDO.h"
#include "tradeapi/ThostFtdcTraderApi.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPCancleOrder::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPCancelOrder::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto pDO = (OrderDO*)reqDO.get();

	auto userinfo = session->getUserInfo();

	CThostFtdcInputOrderActionField req;
	std::memset(&req, 0x0, sizeof(req));

	req.ActionFlag = THOST_FTDC_AF_Delete;
	std::strcpy(req.BrokerID, userinfo->getBrokerId().data());
	std::strcpy(req.InvestorID, userinfo->getInvestorId().data());
	std::strcpy(req.UserID, userinfo->getUserId().data());
	
	if (pDO->OrderSysID != 0)
	{
		std::strcpy(req.ExchangeID, pDO->ExchangeID().data());
		std::sprintf(req.OrderSysID, FMT_PADDING_ORDERSYSID, pDO->OrderSysID);
	}
	else
	{
		req.SessionID = userinfo->getSessionId();
		req.FrontID = userinfo->getFrontId();
		std::strcpy(req.InstrumentID, pDO->InstrumentID().data());
		std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, pDO->OrderID);
	}

	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqOrderAction(&req, ++_requestIdGen);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPCancelOrder::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPCancleOrder::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPCancelOrder::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	dataobj_ptr ret;

	if (rawRespParams.size() > 1)
	{
		auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1];
		auto pData = (CThostFtdcInputOrderActionField*)rawRespParams[0];

		ret = CTPUtility::ParseRawOrderAction(pData, pRsp, OrderStatus::CANCEL_REJECTED);
	}
	else
	{
		auto pData = (CThostFtdcOrderField*)rawRespParams[0];
		ret = CTPUtility::ParseRawOrder(pData);
	}

	return ret;
}