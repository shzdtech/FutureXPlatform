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
// Name:       CTPCancleOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPCancelOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPCancelOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto pDO = (OrderRequestDO*)reqDO.get();

	auto& userInfo = session->getUserInfo();

	CThostFtdcInputOrderActionField req{};
	req.RequestID = serialId;

	req.ActionFlag = THOST_FTDC_AF_Delete;
	std::strncpy(req.BrokerID, userInfo.getBrokerId().data(), sizeof(req.BrokerID));
	std::strncpy(req.InvestorID, userInfo.getInvestorId().data(), sizeof(req.InvestorID));
	std::strncpy(req.UserID, userInfo.getUserId().data(), sizeof(req.UserID));
	
	if (pDO->OrderSysID != 0)
	{
		std::strncpy(req.ExchangeID, pDO->ExchangeID().data(), sizeof(req.ExchangeID));
		std::snprintf(req.OrderSysID, sizeof(req.OrderSysID), FMT_ORDERSYSID, pDO->OrderSysID);
	}
	else
	{
		req.SessionID = userInfo.getSessionId();
		req.FrontID = userInfo.getFrontId();
		std::strncpy(req.InstrumentID, pDO->InstrumentID().data(), sizeof(req.InstrumentID));
		std::snprintf(req.OrderRef, sizeof(req.OrderRef), FMT_ORDERREF, pDO->OrderID);
	}

	int iRet = ((CTPRawAPI*)rawAPI)->TdAPIProxy()->get()->ReqOrderAction(&req, serialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPCancelOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPCancleOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPCancelOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	OrderDO_Ptr ret;
	auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1];
	CTPUtility::CheckError(pRsp);
	if (rawRespParams.size() > 2)
	{
		if (auto pData = (CThostFtdcInputOrderActionField*)rawRespParams[0])
			if (ret = CTPUtility::ParseRawOrder(pData, pRsp))
			{
				if (ret->IsSystemUserId())
				{
					ret->SetUserID(CTPUtility::MakeUserID(ret->BrokerID, ret->InvestorID));
				}
			}
	}
	else
	{
		if (auto pData = (CThostFtdcOrderActionField*)rawRespParams[0])
			if (ret = CTPUtility::ParseRawOrder(pData, pRsp))
			{
				if (ret->IsSystemUserId())
				{
					ret->SetUserID(CTPUtility::MakeUserID(ret->BrokerID, ret->InvestorID));
				}
			}
	}

	return ret;
}