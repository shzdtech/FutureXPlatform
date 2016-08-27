/***********************************************************************
 * Module:  CTPSubMarketData.cpp
 * Author:  milk
 * Modified: 2015年3月6日 20:11:00
 * Purpose: Implementation of the class CTPSubMarketData
 ***********************************************************************/

#include "CTPSubMarketData.h"
#include "CTPRawAPI.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../utility/TUtil.h"
#include "../bizutility/ContractCache.h"
#include "../common/BizErrorIDs.h"

#include <algorithm>
#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPSubMarketData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	int ret = 0;
	if (!stdo->Data.empty())
	{
		auto& instList = stdo->Data.begin()->second;
		auto nInstrument = instList.size();
		if (nInstrument > 0)
		{
			std::unique_ptr<char*[]> ppInstrments(new char*[nInstrument]);
			int i = 0;
			for (auto& inst : instList)
			{
				ppInstrments[i] = const_cast<char*>(inst.data());
				std::string instrument(inst);
				std::transform(inst.begin(), inst.end(), instrument.begin(), ::tolower);
				if (auto pInstumentDO = ContractCache::Get(ProductType::PRODUCT_FUTURE).QueryInstrumentById(instrument))
				{
					ppInstrments[i] = const_cast<char*>(pInstumentDO->InstrumentID().data());
				}
				else
				{
					std::transform(inst.begin(), inst.end(), instrument.begin(), ::toupper);
					if (auto pInstumentDO = ContractCache::Get(ProductType::PRODUCT_FUTURE).QueryInstrumentById(instrument))
					{
						ppInstrments[i] = const_cast<char*>(pInstumentDO->InstrumentID().data());
					}
				}
				i++;
			}
			ret = ((CTPRawAPI*)rawAPI)->MdAPI->SubscribeMarketData(ppInstrments.get(), i);
			CTPUtility::CheckReturnError(ret);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPSubMarketData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPSubMarketData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSubMarketData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	VectorDO_Ptr<MarketDataDO> ret = std::make_shared<VectorDO<MarketDataDO>>();

	ret->HasMore = !*(bool*)rawRespParams[3];

	if (auto pRspInstr = (CThostFtdcSpecificInstrumentField*)rawRespParams[0])
	{
		std::string exchange;
		if (auto pInstumentDO = ContractCache::Get(ProductType::PRODUCT_FUTURE).QueryInstrumentById(pRspInstr->InstrumentID))
		{
			exchange = pInstumentDO->ExchangeID();
		}
		else
		{
			std::string errmsg("Contract code: ");
			throw NotFoundException(errmsg + pRspInstr->InstrumentID + " does not exists.");
		}

		ret->push_back(MarketDataDO(exchange, pRspInstr->InstrumentID));
	}

	return ret;
}