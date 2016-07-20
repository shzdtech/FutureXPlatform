/***********************************************************************
 * Module:  CTPQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:18:21
 * Purpose: Implementation of the class CTPQueryOrder
 ***********************************************************************/

#include "CTPQueryOrder.h"
#include "CTPRawAPI.h"
#include "CTPUtility.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include "../utility/stringutility.h"

#include "../dataobject/OrderDO.h"

#include <boolinq/boolinq.h>

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& investorid = session->getUserInfo()->getInvestorId();
	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
	auto& orderid = stdo->TryFind(STR_ORDER_ID, EMPTY_STRING);
	auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
	auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);

	CThostFtdcQryOrderField req{};
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, investorid.data());
	std::strcpy(req.InstrumentID, instrumentid.data());
	std::strcpy(req.ExchangeID, exchangeid.data());
	std::strcpy(req.OrderSysID, orderid.data());
	std::strcpy(req.InsertTimeStart, tmstart.data());
	std::strcpy(req.InsertTimeEnd, tmend.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryOrder(&req, reqDO->SerialId);
	// CTPUtility::CheckReturnError(iRet);

	if (auto wkProcPtr =
		MessageUtility::FindGlobalProcessor<CTPTradeWorkerProcessor>(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT))
	{
		auto& userMap = wkProcPtr->GetUserOrderMap(session->getUserInfo()->getUserId());
		if (userMap.size() < 1)
		{
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}

		ThrowNotFoundException(&userMap);

		using namespace boolinq;
		if (orderid != EMPTY_STRING)
		{
			auto it = 
				userMap.find(std::strtoull(orderid.data(), nullptr, 0));

			if (it != userMap.end())
			{
				auto orderptr = std::make_shared<OrderDO>(it->second);
				orderptr->SerialId = stdo->SerialId;
				orderptr->HasMore = false;
				wkProcPtr->SendDataObject(session, MSG_ID_QUERY_ORDER, orderptr);
			}
			else
				throw NotFoundException();
		}
		else
		{
			auto lastit = std::prev(userMap.end());
			for (auto it = userMap.begin(); it != userMap.end(); it++)
			{
				auto orderptr = std::make_shared<OrderDO>(it->second);
				orderptr->SerialId = stdo->SerialId;
				orderptr->HasMore = it != lastit;
				wkProcPtr->SendDataObject(session, MSG_ID_QUERY_ORDER, orderptr);
			}
		}
	}

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
	CTPUtility::CheckNotFound(rawRespParams[0]);

	OrderDO_Ptr ret;
	if (auto pData = (CThostFtdcOrderField*)rawRespParams[0])
	{
		ret = CTPUtility::ParseRawOrder(pData);
		ret->SerialId = serialId;
		
		if (rawRespParams.size() > 1)
		{
			if (auto pRsp = (CThostFtdcRspInfoField*)rawRespParams[1])
				ret->ErrorCode = pRsp->ErrorID;
			ret->HasMore = !*(bool*)rawRespParams[3];

			if (auto wkProcPtr =
				MessageUtility::FindGlobalProcessor<CTPTradeWorkerProcessor>(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT))
			{
				wkProcPtr->GetUserOrderMap(ret->UserID()).getorfill(ret->OrderSysID, *ret);
			}
		}
	}

	return ret;
}