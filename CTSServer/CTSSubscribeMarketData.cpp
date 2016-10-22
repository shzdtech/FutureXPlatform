/***********************************************************************
 * Module:  CTSSubscribeMarketData.cpp
 * Author:  milk
 * Modified: 2015年11月8日 22:26:26
 * Purpose: Implementation of the class CTSSubscribeMarketData
 ***********************************************************************/

#include "CTSSubscribeMarketData.h"
#include "CTSAPIWrapper.h"
#include "../common/BizErrorIDs.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"


////////////////////////////////////////////////////////////////////////
// Name:       CTSSubscribeMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSSubscribeMarketData::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSSubscribeMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	auto mdVec = std::make_shared<VectorDO<MarketDataDO>>();

	if (!stdo->Data.size() > 1)
	{
		auto& exchangeList = stdo->Data[STR_EXCHANGE_ID];
		auto& instList = stdo->Data[STR_INSTRUMENT_ID];
		int nInst = instList.size();
		auto api = (CTSAPIWrapper*)rawAPI;
		for (int i = 0; i < nInst; i++)
		{
			if( api->Impl()->Subscribe(exchangeList[i].data(), instList[i].data(), serialId) == 0)
				mdVec->push_back(MarketDataDO(exchangeList[i], instList[i]));
		}
	}

	return mdVec;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSSubscribeMarketData::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSSubscribeMarketData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSSubscribeMarketData::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}