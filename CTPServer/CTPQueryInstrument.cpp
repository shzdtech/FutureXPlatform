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
#include <glog/logging.h>

#include "../common/Attribute_Key.h"
#include "../common/typedefs.h"

#include <boolinq/boolinq.h>

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
	auto& productid = TUtil::FirstNamedEntry(STR_PRODUCT_ID, data, EMPTY_STRING);

	VectorDO_Ptr<InstrumentDO> ret;

	if (instrumentid == EMPTY_STRING && exchangeid == EMPTY_STRING && productid == EMPTY_STRING)
		return ret;

	if (auto pInstrumentMap = AttribPointerCast(session->getProcessor(),
		STR_KEY_SERVER_CONTRACT_DETAIL, InstrumentDOMap))
	{
		ret = std::make_shared<VectorDO<InstrumentDO>>();

		auto it = pInstrumentMap->find(instrumentid);
		if (it != pInstrumentMap->end())
		{
			ret->push_back(it->second);
		}
		else
		{
			using namespace boolinq;

			if (instrumentid != EMPTY_STRING)
			{
				auto enumerator = from(*ret).where([&instrumentid](const InstrumentDO& insDO)
				{ return insDO.InstrumentID().find_first_of(instrumentid) == 0; })._enumerator;

				try { for (;;) ret->push_back(enumerator.nextObject()); }
				catch (EnumeratorEndException &) {}
			}
			else if (productid != EMPTY_STRING)
			{
				auto enumerator = from(*ret).where([&productid](const InstrumentDO& insDO)
				{ return insDO.ProductID.find_first_of(productid) == 0; })._enumerator;

				try { for (;;) ret->push_back(enumerator.nextObject()); }
				catch (EnumeratorEndException &) {}
			}
			else if (exchangeid != EMPTY_STRING)
			{
				auto enumerator = from(*ret).where([&exchangeid](const InstrumentDO& insDO)
				{ return stringutility::compare(insDO.ExchangeID().data(), exchangeid.data()) == 0; })._enumerator;

				try { for (;;) ret->push_back(enumerator.nextObject()); }
				catch (EnumeratorEndException &) {}
			}
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryInstrument::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryInstrument::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryInstrument::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	VectorDO_Ptr<InstrumentDO> ret;

	if (auto pData = (CThostFtdcInstrumentField*)rawRespParams[0])
	{
		InstrumentDO insDO(pData->ExchangeID, pData->InstrumentID);

		insDO.Name = Encoding::ToUTF8(pData->InstrumentName, Encoding::CHARSET_GB2312);
		insDO.ProductID = pData->ProductID;
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
		insDO.LifePhase = pData->InstLifePhase;
		insDO.IsTrading = pData->IsTrading;
		insDO.PositionType = (PositionType)(pData->PositionType - THOST_FTDC_PT_Net);
		insDO.PositionDateType = (PositionDateType)(pData->PositionDateType - THOST_FTDC_PDT_UseHistory);
		insDO.LongMarginRatio = pData->LongMarginRatio;
		insDO.ShortMarginRatio = pData->ShortMarginRatio;
		insDO.MaxMarginSideAlgorithm = pData->MaxMarginSideAlgorithm;

		int reqId = *(int*)rawRespParams[2];
		if (reqId == 0)
			if (auto pInstrumentMap = AttribPointerCast(session->getProcessor(),
				STR_KEY_SERVER_CONTRACT_DETAIL, InstrumentDOMap))
			{
				pInstrumentMap->emplace(insDO.InstrumentID(), insDO);
			}

		ret = std::make_shared<VectorDO<InstrumentDO>>();
		ret->push_back(std::move(insDO));
	}

	return ret;
}