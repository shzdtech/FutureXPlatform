/***********************************************************************
 * Module:  CTPQueryTrade.cpp
 * Author:  milk
 * Modified: 2015年7月11日 18:55:31
 * Purpose: Implementation of the class CTPQueryTrade
 ***********************************************************************/

#include "CTPQueryTrade.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>

#include "../dataobject/TradeRecordDO.h"
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
	auto stdo = (StringTableDO*)reqDO.get();
	auto data = stdo->Data;

	auto& brokeid = session->getUserInfo()->getBrokerId();
	auto& userid = session->getUserInfo()->getInvestorId();
	auto& instrid = TUtil::FirstNamedEntry(STR_INSTRUMENT_ID, data, EMPTY_STRING);
	auto& exchangeid = TUtil::FirstNamedEntry(STR_EXCHANGE_ID, data, EMPTY_STRING);
	auto& tradeid = TUtil::FirstNamedEntry(STR_TRADE_ID, data, EMPTY_STRING);
	auto& tmstart = TUtil::FirstNamedEntry(STR_TIME_START, data, EMPTY_STRING);
	auto& tmend = TUtil::FirstNamedEntry(STR_TIME_END, data, EMPTY_STRING);


	CThostFtdcQryTradeField req;
	std::memset(&req, 0, sizeof(req));
	std::strcpy(req.BrokerID, brokeid.data());
	std::strcpy(req.InvestorID, userid.data());
	std::strcpy(req.InstrumentID, instrid.data());
	std::strcpy(req.ExchangeID, exchangeid.data());
	std::strcpy(req.TradeID, tradeid.data());
	std::strcpy(req.TradeTimeStart, tmstart.data());
	std::strcpy(req.TradeTimeEnd, tmend.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryTrade(&req, ++_requestIdGen);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryTrade::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryTrade::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryTrade::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;
	if (auto pData = (CThostFtdcTradeField*)rawRespParams[0])
	{
		auto pDO = new TradeRecordDO(pData->ExchangeID, pData->InstrumentID);
		ret.reset(pDO);

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