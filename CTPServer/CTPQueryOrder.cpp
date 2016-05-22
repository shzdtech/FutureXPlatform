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
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();
	auto& instrumentid = TUtil::FirstNamedEntry(STR_INSTRUMENT_ID, data, EMPTY_STRING);
	auto& exchangeid = TUtil::FirstNamedEntry(STR_EXCHANGE_ID, data, EMPTY_STRING);
	auto& orderid = TUtil::FirstNamedEntry(STR_ORDER_ID, data, EMPTY_STRING);
	auto& tmstart = TUtil::FirstNamedEntry(STR_TIME_START, data, EMPTY_STRING);
	auto& tmend = TUtil::FirstNamedEntry(STR_TIME_END, data, EMPTY_STRING);

	CThostFtdcQryOrderField req;
	std::memset(&req, 0, sizeof(CThostFtdcQryOrderField));
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	std::strcpy(req.InstrumentID, instrumentid.data());
	std::strcpy(req.ExchangeID, exchangeid.data());
	std::strcpy(req.OrderSysID, orderid.data());
	std::strcpy(req.InsertTimeStart, tmstart.data());
	std::strcpy(req.InsertTimeEnd, tmend.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryOrder(&req, stdo->SerialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryOrder::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryOrder::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryOrder::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	OrderDO_Ptr ret;
	if (auto pData = (CThostFtdcOrderField*)rawRespParams[0])
	{
		ret = CTPUtility::ParseRawOrder(pData);
		ret->SerialId = *(uint32_t*)rawRespParams[2];
		ret->HasMore = *(bool*)rawRespParams[3];

		if (rawRespParams.size() > 1)
		{
			if (auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1])
				ret->ErrorCode = pRsp->ErrorID;
		}
	}

	return ret;
}