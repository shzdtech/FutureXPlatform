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
// Name:       CTPCancleOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPCancelOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto pDO = (OrderRequestDO*)reqDO.get();

	auto userinfo = session->getUserInfo();

	CThostFtdcInputOrderActionField req{};
	req.RequestID = serialId;

	req.ActionFlag = THOST_FTDC_AF_Delete;
	std::strncpy(req.BrokerID, userinfo->getBrokerId().data(), sizeof(req.BrokerID) - 1);
	std::strncpy(req.InvestorID, userinfo->getInvestorId().data(), sizeof(req.InvestorID) - 1);
	std::strncpy(req.UserID, userinfo->getUserId().data(), sizeof(req.UserID) - 1);
	
	if (pDO->OrderSysID != 0)
	{
		std::strncpy(req.ExchangeID, pDO->ExchangeID().data(), sizeof(req.ExchangeID) - 1);;
		std::sprintf(req.OrderSysID, FMT_PADDING_ORDERSYSID, pDO->OrderSysID);
	}
	else
	{
		req.SessionID = userinfo->getSessionId();
		req.FrontID = userinfo->getFrontId();
		std::strncpy(req.InstrumentID, pDO->InstrumentID().data(), sizeof(req.InstrumentID) - 1);
		std::sprintf(req.OrderRef, FMT_PADDING_ORDERREF, pDO->OrderID);
	}

	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqOrderAction(&req, serialId);
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
	auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1];
	CTPUtility::CheckError(pRsp);
	if (rawRespParams.size() > 2)
	{
		if (auto pData = (CThostFtdcInputOrderActionField*)rawRespParams[0])
			ret = CTPUtility::ParseRawOrder(pData, pRsp);
	}
	else
	{
		if (auto pData = (CThostFtdcOrderActionField*)rawRespParams[0])
			ret = CTPUtility::ParseRawOrder(pData, pRsp);
	}

	return ret;
}