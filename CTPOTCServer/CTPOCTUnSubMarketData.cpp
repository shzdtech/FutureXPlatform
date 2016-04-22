/***********************************************************************
 * Module:  CTPOCTUnSubMarketData.cpp
 * Author:  milk
 * Modified: 2016年4月19日 12:59:04
 * Purpose: Implementation of the class CTPOCTUnSubMarketData
 ***********************************************************************/

#include "CTPOCTUnSubMarketData.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/Attribute_Key.h"
#include "../CTPServer/CTPAppContext.h"
#include "CTPOTCWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include <glog/logging.h>

////////////////////////////////////////////////////////////////////////
// Name:       CTPOCTUnSubMarketData::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOCTUnSubMarketData::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOCTUnSubMarketData::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckLogin(session);

	auto userContractMap = std::static_pointer_cast<UserContractParamMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

	if (auto workPrc = CTPAppContext::FindServerProcessor
		(CTPWorkProcessorID::WORKPROCESSOR_OTC))
	{
		auto otcworkproc = (std::static_pointer_cast<CTPOTCWorkerProcessor>(workPrc));
		for (auto it = userContractMap->begin(); it != userContractMap->end(); it++)
		{
			otcworkproc->UnRegisterPricingListener(it->first, (IMessageSession*)session);
		}

		userContractMap->clear();
	}

	dataobj_ptr ret;
	auto stDO = new TMultiRecordDO < std::string >;
	ret.reset(stDO);
	stDO->EOFFlag = true;
	DLOG(INFO) << "OTCUnsubMarketData successful." << std::endl;

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOCTUnSubMarketData::HandleResponse(ParamVector rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOCTUnSubMarketData::HandleResponse()
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOCTUnSubMarketData::HandleResponse(ParamVector rawParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}