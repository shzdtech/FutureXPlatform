/***********************************************************************
 * Module:  CTPUnsubMarketData.cpp
 * Author:  milk
 * Modified: 2015年3月6日 20:11:00
 * Purpose: Implementation of the class CTPUnsubMarketData
 ***********************************************************************/

#include "CTPUnsubMarketData.h"
#include "CTPRawAPI.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../message/BizError.h"
#include <boost/locale/encoding.hpp>
#include "CTPUtility.h"
#include <algorithm>

////////////////////////////////////////////////////////////////////////
// Name:       CTPUnsubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPUnsubMarketData::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPUnsubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto stdo = (StringTableDO*)reqDO.get();
	int ret = 0;
	if (!stdo->Data.empty())
	{
		auto& instList = stdo->Data.begin()->second;
		auto nInstrument = instList.size();
		if (nInstrument > 0)
		{
			auto ppInstrments = new char*[nInstrument];
			std::unique_ptr<char*[]> instrmentsPtr(ppInstrments);
			for (int i = 0; i < nInstrument; i++)
			{
				ppInstrments[i] = const_cast<char*>(instList[i].data());
			}
			ret = ((CTPRawAPI*)rawAPI)->MdAPI->UnSubscribeMarketData(ppInstrments, nInstrument);
			CTPUtility::CheckReturnError(ret);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPUnsubMarketData::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPUnsubMarketData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPUnsubMarketData::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;
	auto pRspInstr = (CThostFtdcSpecificInstrumentField*)rawRespParams[0];

	auto stDO = new StringTableDO;
	ret.reset(stDO);
	stDO->Data[STR_INSTRUMENT_ID].push_back(pRspInstr->InstrumentID);
	stDO->HasMore = !*(bool*)rawRespParams[3];

	return ret;
}