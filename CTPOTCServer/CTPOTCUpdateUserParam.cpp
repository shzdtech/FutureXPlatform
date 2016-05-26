/***********************************************************************
 * Module:  CTPOTCUpdateUserParam.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:50:10
 * Purpose: Implementation of the class CTPOTCUpdateUserParam
 ***********************************************************************/

#include "CTPOTCUpdateUserParam.h"

#include "../common/Attribute_Key.h"

#include  "../message/message_macro.h"
#include "../message/MessageProcessor.h"
#include "../message/DefMessageID.h"
#include "../message/BizError.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/ResultDO.h"
#include "../pricingengine/IPricingDataContext.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateUserParam::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateUserParam::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateUserParam::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

	auto vecUserConDO_Ptr = (VectorDO<UserContractParam>*)reqDO.get();

	auto strategyMap = AttribPointerCast(session->getProcessor(),
		STR_KEY_SERVER_PRICING_DATACONTEXT, IPricingDataContext)->GetStrategyMap();

	for (auto& userConDO : *vecUserConDO_Ptr)
	{
		auto& userContract = userContractMap_Ptr->at(userConDO);
		userContract.Quantity = userConDO.Quantity;

		auto it = strategyMap->find(userConDO);
		if (it != strategyMap->end())
		{
			OnResponseProcMacro(
				session->getProcessor(),
				MSG_ID_RTN_PRICING, 
				reqDO->SerialId,
				&it->second);
		}
	}

	return std::make_shared<ResultDO>(reqDO->SerialId);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateUserParam::HandleResponse(const uint32_t serialId, paramvector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateUserParam::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateUserParam::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}