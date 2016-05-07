/***********************************************************************
 * Module:  CTPQueryInstrument.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:06:13
 * Purpose: Implementation of the class CTPQueryInstrument
 ***********************************************************************/

#include "CTPQueryInstrument.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"

#include "../message/BizError.h"

#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>

#include "../dataobject/InstrumentDO.h"
#include "CTPUtility.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryInstrument::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryInstrument::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryInstrument::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;

	auto& instrumentid = TUtil::FirstNamedEntry(STR_INSTRUMENT_ID, data, EMPTY_STRING);
	auto& exchangeid = TUtil::FirstNamedEntry(STR_EXCHANGE_ID, data, EMPTY_STRING);
	auto& exchginstrid = TUtil::FirstNamedEntry(STR_EXCHANGE_INSTRUMENT_ID, data, EMPTY_STRING);
	auto& productid = TUtil::FirstNamedEntry(STR_PRODUCT_ID, data, EMPTY_STRING);

	CThostFtdcQryInstrumentField req;
	std::memset(&req, 0x0, sizeof(CThostFtdcQryInstrumentField));
	std::strcpy(req.InstrumentID, instrumentid.data());
	std::strcpy(req.ExchangeID, exchangeid.data());
	std::strcpy(req.ExchangeInstID, exchginstrid.data());
	std::strcpy(req.ProductID, productid.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryInstrument(&req, ++_requestIdGen);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryInstrument::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryInstrument::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryInstrument::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);
	dataobj_ptr ret;

	if (auto pData = (CThostFtdcInstrumentField*)rawRespParams[0])
	{
		auto pDO = new InstrumentDO(pData->ExchangeID, pData->InstrumentID);
		ret.reset(pDO);

		pDO->Name = Encoding::ToUTF8(pData->InstrumentName, Encoding::CHARSET_GB2312);
		pDO->ProductID = pData->ProductID;
		pDO->ProductType = (ProductType)(pData->ProductClass - THOST_FTDC_PC_Futures);
		pDO->DeliveryYear = pData->DeliveryYear;
		pDO->DeliveryMonth = pData->DeliveryMonth;
		pDO->MaxMarketOrderVolume = pData->MaxMarketOrderVolume;
		pDO->MinMarketOrderVolume = pData->MinMarketOrderVolume;
		pDO->MaxLimitOrderVolume = pData->MaxLimitOrderVolume;
		pDO->MinLimitOrderVolume = pData->MinLimitOrderVolume;
		pDO->VolumeMultiple = pData->VolumeMultiple;
		pDO->PriceTick = pData->PriceTick;
		pDO->CreateDate = pData->CreateDate;
		pDO->OpenDate = pData->OpenDate;
		pDO->ExpireDate = pData->ExpireDate;
		pDO->StartDelivDate = pData->StartDelivDate;
		pDO->EndDelivDate = pData->EndDelivDate;
		pDO->LifePhase = pData->InstLifePhase;
		pDO->IsTrading = pData->IsTrading;
		pDO->PositionType = (PositionType)(pData->PositionType - THOST_FTDC_PT_Net);
		pDO->PositionDateType = (PositionDateType)(pData->PositionDateType - THOST_FTDC_PDT_UseHistory);
		pDO->LongMarginRatio = pData->LongMarginRatio;
		pDO->ShortMarginRatio = pData->ShortMarginRatio;
		pDO->MaxMarginSideAlgorithm = pData->MaxMarginSideAlgorithm;
	}

	return ret;
}