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
#include "../bizutility/InstrumentCache.h"

#include <glog/logging.h>
#include <algorithm>
#include "CTPUtility.h"
 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPSubMarketData::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPSubMarketData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     void
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPSubMarketData::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	int ret = 0;
	if (stdo->Data.size() > 0)
	{
		auto& instList = stdo->Data[STR_INSTRUMENT_ID];
		if (instList.size() > 0)
		{
			char* pContract[1];
			for (auto& inst : instList)
			{
				std::transform(inst.begin(), inst.end(), inst.begin(), ::tolower);
				pContract[0] = const_cast<char*>(inst.data());
				ret = ((CTPRawAPI*)rawAPI)->MdAPI->SubscribeMarketData(pContract, 1);
				CTPUtility::CheckReturnError(ret);

				std::transform(inst.begin(), inst.end(), inst.begin(), ::toupper);
				pContract[0] = const_cast<char*>(inst.data());
				ret = ((CTPRawAPI*)rawAPI)->MdAPI->SubscribeMarketData(pContract, 1);
				CTPUtility::CheckReturnError(ret);
			}
		}
	}
	DLOG(INFO) << "SubMarketData" << std::endl;

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
	ret->SerialId = serialId;
	ret->HasMore = *(bool*)rawRespParams[3];

	if (auto pRspInstr = (CThostFtdcSpecificInstrumentField*)rawRespParams[0])
	{
		std::string exchange;

		if (auto instruments = InstrumentCache::QueryInstrument(pRspInstr->InstrumentID))
		{
			if (instruments->size() > 0)
				exchange = instruments->at(0).ExchangeID();
		}

		ret->push_back(MarketDataDO(exchange, pRspInstr->InstrumentID));

		DLOG(INFO) << "Subcrible InstID: " << pRspInstr->InstrumentID << std::endl;
	}

	return ret;
}