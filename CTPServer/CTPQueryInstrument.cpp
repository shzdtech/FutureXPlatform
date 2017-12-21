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
#include "../dataobject/DateType.h"
#include "../databaseop/ContractDAO.h"

#include "CTPTradeWorkerProcessor.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/IMessageServiceLocator.h"


#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"
#include "../utility/stringutility.h"

#include "../bizutility/ContractCache.h"
#include "../bizutility/UnderlyingMap.h"

#include "CTPUtility.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPQueryInstrument::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of CTPQueryInstrument::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryInstrument::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto stdo = (StringMapDO<std::string>*)reqDO.get();

	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);
	auto& productid = stdo->TryFind(STR_PRODUCT_ID, EMPTY_STRING);
	auto& instrumentCache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);

	VectorDO_Ptr<InstrumentDO> ret;

	if (instrumentid == EMPTY_STRING && exchangeid == EMPTY_STRING && productid == EMPTY_STRING)
		ret = instrumentCache.AllInstruments();
	else
		ret = instrumentCache.QueryInstrument(instrumentid, exchangeid, productid);

	if (TUtil::IsNullOrEmpty(ret))
	{
		bool loaded = false;
		if (auto tradeAPIProxy = ((CTPRawAPI*)rawAPI)->TdAPIProxy())
		{
			CThostFtdcQryInstrumentField req{};
			std::strncpy(req.ExchangeID, exchangeid.data(), sizeof(req.ExchangeID));
			std::strncpy(req.InstrumentID, instrumentid.data(), sizeof(req.InstrumentID));
			std::strncpy(req.ProductID, productid.data(), sizeof(req.ProductID));
			auto retCode = tradeAPIProxy->get()->ReqQryInstrument(&req, serialId);
			loaded = retCode == 0;
		}

		if (!loaded)
		{
			std::string productTypes;
			if (msgProcessor->getServerContext()->getConfigVal("product_type", productTypes))
			{
				std::vector<std::string> productVec;
				stringutility::split(productTypes, productVec);

				for (auto productType : productVec)
				{
					auto vectPtr = ContractDAO::FindContractByProductType(std::stoi(productType));
					for (auto& contract : *vectPtr)
						instrumentCache.Add(contract);
				}
			}
		}

		if (instrumentid == EMPTY_STRING && exchangeid == EMPTY_STRING && productid == EMPTY_STRING)
			ret = instrumentCache.AllInstruments();
		else
			ret = instrumentCache.QueryInstrument(instrumentid, exchangeid, productid);

		if (TUtil::IsNullOrEmpty(ret))
			throw NotFoundException();
	}

	ret->HasMore = false;
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryInstrument::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTPQueryInstrument::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryInstrument::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CTPUtility::CheckNotFound(rawRespParams[0]);
	CTPUtility::CheckError(rawRespParams[1]);

	// VectorDO_Ptr<InstrumentDO> ret = std::make_shared<VectorDO<InstrumentDO>>();

	// ret->HasMore = !*(bool*)rawRespParams[3];

	if (auto pData = (CThostFtdcInstrumentField*)rawRespParams[0])
	{
		InstrumentDO insDO(pData->ExchangeID, pData->InstrumentID);

		insDO.Name = boost::locale::conv::to_utf<char>(pData->InstrumentName, CHARSET_GB2312);
		insDO.ProductType = (ProductType)(pData->ProductClass - THOST_FTDC_PC_Futures);
		insDO.ProductID = pData->ProductID;
		insDO.ContractType = ContractType::CONTRACTTYPE_FUTURE;
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
		DateType::YYYYMMDD2YYYY_MM_DD(pData->ExpireDate, insDO.ExpireDate);
		insDO.StartDelivDate = pData->StartDelivDate;
		insDO.EndDelivDate = pData->EndDelivDate;
		insDO.LifePhase = (LifePhaseType)(pData->InstLifePhase - THOST_FTDC_IP_NotStart);
		insDO.IsTrading = pData->IsTrading;
		insDO.PositionType = (PositionType)(pData->PositionType - THOST_FTDC_PT_Net);
		insDO.PositionDateType = (PositionDateType)(pData->PositionDateType - THOST_FTDC_PDT_UseHistory);
		insDO.LongMarginRatio = pData->LongMarginRatio;
		insDO.ShortMarginRatio = pData->ShortMarginRatio;
		insDO.StrikePrice = 0;

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
				break;
			}
		}

		UnderlyingMap::TryMap(insDO, insDO.ProductID);

		if (!ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(pData->InstrumentID))
		{
			bool cache = true;
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
			{
				auto& productSet = pWorkerProc->GetProductTypeToLoad();
				cache = productSet.find(insDO.ProductType) != productSet.end();
			}

			if (cache)
				ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).Add(insDO);
		}

		// ret->push_back(std::move(insDO));
	}

	return nullptr;
}