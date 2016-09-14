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

#include "../message/message_macro.h"
#include "../message/DefMessageID.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPCancleOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPCancelOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto pDO = (OrderRequestDO*)reqDO.get();

	auto userinfo = session->getUserInfo();

	CThostFtdcInputOrderActionField req{};

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

	bool bLast = true;
	OnResponseProcMacro(session->getProcessor(), MSG_ID_ORDER_CANCEL, reqDO->SerialId, &req, nullptr, &reqDO->SerialId, &bLast);

	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqOrderAction(&req, reqDO->SerialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPCancelOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPCancleOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPCancelOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	dataobj_ptr ret;

	if (rawRespParams.size() > 2)
	{
		auto pData = (CThostFtdcInputOrderActionField*)rawRespParams[0];
		auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1];
		ret = CTPUtility::ParseRawOrderInputAction(pData, pRsp);
	}
	else
	{
		auto pData = (CThostFtdcOrderActionField*)rawRespParams[0];
		auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1];
		ret = CTPUtility::ParseRawOrderAction(pData, pRsp);
	}

	ret->HasMore = false;

	return ret;
}