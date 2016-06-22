/***********************************************************************
 * Module:  CTPQueryTrade.cpp
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Implementation of the class CTPQueryTrade
 ***********************************************************************/

#include "CTPQueryTrade.h"
#include "CTPRawAPI.h"

#include "../common/Attribute_Key.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>

#include "CTPUtility.h"
#include "CTPConstant.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTrade::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryTrade::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTrade::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();
	auto& instrid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
	auto& tradeid = stdo->TryFind(STR_TRADE_ID, EMPTY_STRING);
	auto& tmstart = stdo->TryFind(STR_TIME_START, EMPTY_STRING);
	auto& tmend = stdo->TryFind(STR_TIME_END, EMPTY_STRING);


	CThostFtdcQryTradeField req;
	std::memset(&req, 0, sizeof(req));
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	std::strcpy(req.InstrumentID, instrid.data());
	std::strcpy(req.ExchangeID, exchangeid.data());
	std::strcpy(req.TradeID, tradeid.data());
	std::strcpy(req.TradeTimeStart, tmstart.data());
	std::strcpy(req.TradeTimeEnd, tmend.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryTrade(&req, reqDO->SerialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTrade::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryTrade::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTrade::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;
	if (auto pData = (CThostFtdcTradeField*)rawRespParams[0])
	{
		auto pDO = new TradeRecordDO(pData->ExchangeID, pData->InstrumentID);
		ret.reset(pDO);
		pDO->SerialId = serialId;
		pDO->HasMore = *(bool*)rawRespParams[3];

		pDO->OrderID = std::strtoull(pData->OrderRef, nullptr, 0);
		pDO->OrderSysID = std::strtoull(pData->OrderSysID, nullptr, 0);
		pDO->Direction = pData->Direction == THOST_FTDC_D_Buy ?
			DirectionType::BUY : DirectionType::SELL;
		pDO->OpenClose = (OrderOpenCloseType)(pData->OffsetFlag - THOST_FTDC_OF_Open);
		pDO->Price = pData->Price;
		pDO->Volume = pData->Volume;
		pDO->TradeDate = pData->TradeDate;
		pDO->TradeTime = pData->TradeTime;
		pDO->TradeType = (TradingType)pData->TradeType;
		pDO->HedgeFlag = (HedgeType)(pData->HedgeFlag - THOST_FTDC_HF_Speculation);
	}

	return ret;
}