/***********************************************************************
 * Module:  CTPQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:18:21
 * Purpose: Implementation of the class CTPQueryOrder
 ***********************************************************************/

#include "CTPQueryOrder.h"
#include "CTPRawAPI.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>

#include "../dataobject/OrderDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryOrder::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryOrder::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();
	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
	auto& orderid = stdo->TryFind(STR_ORDER_ID, EMPTY_STRING);
	auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
	auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);

	CThostFtdcQryOrderField req;
	std::memset(&req, 0, sizeof(CThostFtdcQryOrderField));
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	std::strcpy(req.InstrumentID, instrumentid.data());
	std::strcpy(req.ExchangeID, exchangeid.data());
	std::strcpy(req.OrderSysID, orderid.data());
	std::strcpy(req.InsertTimeStart, tmstart.data());
	std::strcpy(req.InsertTimeEnd, tmend.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryOrder(&req, reqDO->SerialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryOrder::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryOrder::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	OrderDO_Ptr ret;
	if (auto pData = (CThostFtdcOrderField*)rawRespParams[0])
	{
		ret = CTPUtility::ParseRawOrder(pData);
		ret->SerialId = serialId;
		ret->HasMore = *(bool*)rawRespParams[3];

		if (rawRespParams.size() > 1)
		{
			if (auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1])
				ret->ErrorCode = pRsp->ErrorID;
		}
	}

	return ret;
}