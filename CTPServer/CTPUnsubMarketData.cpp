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
#include "../utility/Encoding.h"
#include <glog/logging.h>
#include "CTPUtility.h"
#include <algorithm>

////////////////////////////////////////////////////////////////////////
// Name:       CTPUnsubMarketData::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPUnsubMarketData::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPUnsubMarketData::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	int ret = 0;
	if (stdo->Data.size() > 0)
	{
		auto& instList = stdo->Data[STR_INSTRUMENT_ID];
		char* pContract[1];
		for (auto& inst : instList)
		{
			std::transform(inst.begin(), inst.end(), inst.begin(), ::tolower);
			pContract[0] = const_cast<char*>(inst.data());
			ret = ((CTPRawAPI*)rawAPI)->MdAPI->UnSubscribeMarketData(pContract, 1);
			CTPUtility::CheckReturnError(ret);

			std::transform(inst.begin(), inst.end(), inst.begin(), ::toupper);
			pContract[0] = const_cast<char*>(inst.data());
			ret = ((CTPRawAPI*)rawAPI)->MdAPI->UnSubscribeMarketData(pContract, 1);
			CTPUtility::CheckReturnError(ret);
		}
	}
	DLOG(INFO) << "UnsubMarketData" << std::endl;

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPUnsubMarketData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPUnsubMarketData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPUnsubMarketData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;
	auto pRspInstr = (CThostFtdcSpecificInstrumentField*)rawRespParams[0];

	auto stDO = new StringTableDO;
	ret.reset(stDO);
	stDO->Data[STR_INSTRUMENT_ID].push_back(pRspInstr->InstrumentID);
	stDO->SerialId = serialId;
	stDO->HasMore = !*(bool*)rawRespParams[3];
	DLOG(INFO) << "UnsubMarketData successful." << std::endl;

	return ret;
}