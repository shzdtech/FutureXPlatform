/***********************************************************************
 * Module:  CTPOCTUnSubMarketData.cpp
 * Author:  milk
 * Modified: 2016年4月19日 12:59:04
 * Purpose: Implementation of the class CTPOCTUnSubMarketData
 ***********************************************************************/

#include "CTPOCTUnSubMarketData.h"

#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPWorkerProcessorID.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOCTUnSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOCTUnSubMarketData::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOCTUnSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto userContractMap = std::static_pointer_cast<UserContractParamMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

	if (auto wkProcPtr =
		MessageUtility::FindGlobalProcessor<CTPOTCWorkerProcessor>(CTPWorkerProcessorID::WORKPROCESSOR_OTC))
	{
		for (auto it = userContractMap->begin(); it != userContractMap->end(); it++)
		{
			wkProcPtr->UnregisterPricingListener(it->first,
				session->getProcessor()->LockMessageSession().get());
		}

		userContractMap->clear();
	}

	dataobj_ptr ret;
	auto stDO = new TDataObject<std::string>;
	ret.reset(stDO);
	stDO->SerialId = reqDO->SerialId;

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOCTUnSubMarketData::HandleResponse(const uint32_t serialId, param_vector rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOCTUnSubMarketData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOCTUnSubMarketData::HandleResponse(const uint32_t serialId, param_vector rawParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}