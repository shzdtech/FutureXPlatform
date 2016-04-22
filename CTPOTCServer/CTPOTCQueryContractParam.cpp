/***********************************************************************
 * Module:  CTPOTCQueryContractParam.cpp
 * Author:  milk
 * Modified: 2015年8月23日 9:48:32
 * Purpose: Implementation of the class CTPOTCQueryContractParam
 ***********************************************************************/

#include "CTPOTCQueryContractParam.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/Attribute_Key.h"

#include <glog/logging.h>
#include "../strategy/PricingContext.h"

#include "../message/BizError.h"

#include "../dataobject/TemplateDO.h"

#include "../databaseop/ContractDAO.h"


////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryContractParam::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryContractParam::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryContractParam::HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckLogin(session);

	auto cpVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
		session->getContext()->getAttribute(STR_KEY_USER_CONTRACT_PARAM));

	auto contractVec_Ptr = std::make_shared<VectorDO<ContractDO>>();

	auto contractMap = PricingContext::GetContractMap();

	for (auto& con : *cpVec_Ptr)
	{
		auto& contractDO = contractMap->at(con);
		contractVec_Ptr->push_back(contractDO);
	}

	return contractVec_Ptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryContractParam::HandleResponse(ParamVector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryContractParam::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryContractParam::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}