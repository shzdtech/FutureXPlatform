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
#include "../dataobject/TypedefDO.h"

#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/IMessageServiceLocator.h"

#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include "../utility/stringutility.h"

#include "../bizutility/ContractCache.h"
#include "../bizutility/UnderlyingMap.h"

#include "CTPUtility.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryInstrument::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPQueryInstrument::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryInstrument::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
	auto& productid = stdo->TryFind(STR_PRODUCT_ID, EMPTY_STRING);

	VectorDO_Ptr<InstrumentDO> ret;

	if (instrumentid == EMPTY_STRING && exchangeid == EMPTY_STRING && productid == EMPTY_STRING)
		ret = ContractCache::Get(ProductType::PRODUCT_FUTURE).AllInstruments();
	else
		ret = ContractCache::Get(ProductType::PRODUCT_FUTURE).QueryInstrument(instrumentid, exchangeid, productid);

	if (TUtil::IsNullOrEmpty(ret))
	{
		CThostFtdcQryInstrumentField req{};
		std::strcpy(req.ExchangeID, exchangeid.data());
		std::strcpy(req.InstrumentID, instrumentid.data());
		std::strcpy(req.ProductID, productid.data());
		auto retCode = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryInstrument(&req, reqDO->SerialId);
		// CTPUtility::CheckReturnError(retCode);

		throw NotFoundException();
	}

	ret->HasMore = false;
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryInstrument::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryInstrument::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryInstrument::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	VectorDO_Ptr<InstrumentDO> ret = std::make_shared<VectorDO<InstrumentDO>>();

	ret->HasMore = !*(bool*)rawRespParams[3];

	if (auto pData = (CThostFtdcInstrumentField*)rawRespParams[0])
	{
		InstrumentDO insDO(pData->ExchangeID, pData->InstrumentID);

		insDO.Name = Encoding::ToUTF8(pData->InstrumentName, CHARSET_GB2312);
		UnderlyingMap::TryFind(pData->ProductID, insDO.ProductID);
		insDO.ProductType = (ProductType)(pData->ProductClass - THOST_FTDC_PC_Futures);
		insDO.DeliveryYear = pData->DeliveryYear;
		insDO.DeliveryMonth = pData->DeliveryMonth;
		insDO.MaxMarketOrderVolume = pData->MaxMarketOrderVolume;
		insDO.MinMarketOrderVolume = pData->MinMarketOrderVolume;
		insDO.MaxLimitOrderVolume = pData->MaxLimitOrderVolume;
		insDO.MinLimitOrderVolume = pData->MinLimitOrderVolume;
		insDO.VolumeMultiple = pData->VolumeMultiple;
		insDO.PriceTick = pData->PriceTick;
		insDO.CreateDate = pData->CreateDate;
		insDO.OpenDate = pData->OpenDate;
		insDO.ExpireDate = pData->ExpireDate;
		insDO.StartDelivDate = pData->StartDelivDate;
		insDO.EndDelivDate = pData->EndDelivDate;
		insDO.LifePhase = (LifePhaseType)(pData->InstLifePhase - THOST_FTDC_IP_NotStart);
		insDO.IsTrading = pData->IsTrading;
		insDO.PositionType = (PositionType)(pData->PositionType - THOST_FTDC_PT_Net);
		insDO.PositionDateType = (PositionDateType)(pData->PositionDateType - THOST_FTDC_PDT_UseHistory);
		insDO.LongMarginRatio = pData->LongMarginRatio;
		insDO.ShortMarginRatio = pData->ShortMarginRatio;

		if (pData->StrikePrice > 0 && pData->StrikePrice < 1e32)
		{
			insDO.StrikePrice = pData->StrikePrice;

			insDO.UnderlyingContract = ContractKey(pData->ExchangeID, pData->UnderlyingInstrID);

			switch (pData->OptionsType)
			{
			case THOST_FTDC_CP_CallOptions:
				insDO.ContractType = ContractType::CONTRACTTYPE_CALL_OPTION;
				break;
			case THOST_FTDC_CP_PutOptions:
				insDO.ContractType = ContractType::CONTRACTTYPE_PUT_OPTION;
				break;
			default:
				insDO.ContractType = ContractType::CONTRACTTYPE_FUTURE;
			}
		}

		if (!ContractCache::Get(ProductType::PRODUCT_FUTURE).QueryInstrumentById(pData->InstrumentID))
		{
			ContractCache::Get(ProductType::PRODUCT_FUTURE).Add(insDO);
		}

		ret->push_back(std::move(insDO));
	}

	return ret;
}