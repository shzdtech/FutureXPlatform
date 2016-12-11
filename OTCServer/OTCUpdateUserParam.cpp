/***********************************************************************
 * Module:  OTCUpdateUserParam.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:50:10
 * Purpose: Implementation of the class OTCUpdateUserParam
 ***********************************************************************/

#include "OTCUpdateUserParam.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../common/Attribute_Key.h"

#include  "../message/message_macro.h"
#include "../message/MessageProcessor.h"
#include "../message/DefMessageID.h"
#include "../message/MessageUtility.h"
#include "../message/BizError.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/ResultDO.h"
#include "../pricingengine/IPricingDataContext.h"
////////////////////////////////////////////////////////////////////////
// Name:       OTCUpdateUserParam::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of OTCUpdateUserParam::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCUpdateUserParam::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

	auto vecUserConDO_Ptr = (VectorDO<UserContractParamDO>*)reqDO.get();


	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto strategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();

		for (auto& userConDO : *vecUserConDO_Ptr)
		{
			if (auto pUserContract = userContractMap_Ptr->tryfind(userConDO))
			{
				pUserContract->Quantity = userConDO.Quantity;

				auto it = strategyMap->find(userConDO);
				if (it != strategyMap->end())
				{
					OnResponseProcMacro(
						msgProcessor,
						MSG_ID_RTN_PRICING,
						serialId,
						&it->second);
				}
			}
		}
	}

	return std::make_shared<ResultDO>(serialId);
}
