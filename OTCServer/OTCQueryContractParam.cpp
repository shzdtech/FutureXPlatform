/***********************************************************************
 * Module:  OTCQueryContractParam.cpp
 * Author:  milk
 * Modified: 2015年8月23日 9:48:32
 * Purpose: Implementation of the class OTCQueryContractParam
 ***********************************************************************/

#include "OTCQueryContractParam.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../common/Attribute_Key.h"

#include "../message/BizError.h"
#include "../message/MessageUtility.h"

#include "../message/message_macro.h"

#include "../dataobject/TemplateDO.h"

#include "../databaseop/ContractDAO.h"

#include "../pricingengine/IPricingDataContext.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCQueryContractParam::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCQueryContractParam::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryContractParam::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto cpVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
		session->getContext()->getAttribute(STR_KEY_USER_CONTRACT_PARAM));

	ThrowNotFoundExceptionIfEmpty(cpVec_Ptr);

	auto contractVec_Ptr = std::make_shared<VectorDO<ContractParamDO>>();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto contractMap = pWorkerProc->PricingDataContext()->GetContractParamMap();

		for (auto& con : *cpVec_Ptr)
		{
			if (auto pContractDO = contractMap->tryfind(con))
				contractVec_Ptr->push_back(*pContractDO);
		}

		ThrowNotFoundExceptionIfEmpty(contractVec_Ptr);
	}

	return contractVec_Ptr;
}