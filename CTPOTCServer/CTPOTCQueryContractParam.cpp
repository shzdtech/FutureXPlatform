/***********************************************************************
 * Module:  CTPOTCQueryContractParam.cpp
 * Author:  milk
 * Modified: 2015年8月23日 9:48:32
 * Purpose: Implementation of the class CTPOTCQueryContractParam
 ***********************************************************************/

#include "CTPOTCQueryContractParam.h"

#include "../common/Attribute_Key.h"

#include "../message/BizError.h"

#include "../message/message_macro.h"

#include "../dataobject/TemplateDO.h"

#include "../databaseop/ContractDAO.h"

#include "../pricingengine/IPricingDataContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryContractParam::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryContractParam::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryContractParam::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto cpVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
		session->getContext()->getAttribute(STR_KEY_USER_CONTRACT_PARAM));

	auto contractVec_Ptr = std::make_shared<VectorDO<ContractDO>>();
	contractVec_Ptr->SerialId = reqDO->SerialId;

	auto pricingCtx = AttribPointerCast(session->getProcessor(),
		STR_KEY_SERVER_PRICING_DATACONTEXT, IPricingDataContext);

	auto contractMap = pricingCtx->GetContractMap();

	for (auto& con : *cpVec_Ptr)
	{
		auto& contractDO = contractMap->at(con);
		contractVec_Ptr->push_back(contractDO);
	}

	return contractVec_Ptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryContractParam::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryContractParam::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryContractParam::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}