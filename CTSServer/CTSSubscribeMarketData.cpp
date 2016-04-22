/***********************************************************************
 * Module:  CTSSubscribeMarketData.cpp
 * Author:  milk
 * Modified: 2015年11月8日 22:26:26
 * Purpose: Implementation of the class CTSSubscribeMarketData
 ***********************************************************************/

#include "CTSSubscribeMarketData.h"
#include "CTSAPIWrapper.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"


////////////////////////////////////////////////////////////////////////
// Name:       CTSSubscribeMarketData::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSSubscribeMarketData::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSSubscribeMarketData::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	if (stdo->Data.size() > 0)
	{
		auto& exchangeList = stdo->Data[STR_EXCHANGE_ID];
		auto& instList = stdo->Data[STR_INSTRUMENT_ID];
		int nInst = instList.size();
		auto api = (CTSAPIWrapper*)rawAPI;
		for (int i = 0; i < nInst; i++)
		{
			api->Impl()->Subscribe(exchangeList[i].data(), instList[i].data());
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSSubscribeMarketData::HandleResponse(ParamVector& rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSSubscribeMarketData::HandleResponse()
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSSubscribeMarketData::HandleResponse(ParamVector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}