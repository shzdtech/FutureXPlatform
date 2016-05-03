/***********************************************************************
 * Module:  CTPOTCUpdateUserParam.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:50:10
 * Purpose: Implementation of the class CTPOTCUpdateUserParam
 ***********************************************************************/

#include "CTPOTCUpdateUserParam.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/Attribute_Key.h"
#include "../strategy/PricingContext.h"

#include  "../message/message_marco.h"
#include "../message/MessageProcessor.h"
#include "../message/DefMessageID.h"
#include "../message/BizError.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/UserContractParam.h"
#include "../dataobject/ResultDO.h"

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
	CTPUtility::CheckLogin(session);

	auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

	auto vecUserConDO_Ptr = (VectorDO<UserContractParam>*)reqDO.get();

	auto strategyMap = PricingContext::GetStrategyMap();

	for (auto& userConDO : *vecUserConDO_Ptr)
	{
		auto& userContract = userContractMap_Ptr->at(userConDO);
		userContract.Quantity = userConDO.Quantity;

		auto it = strategyMap->find(userConDO);
		if (it != strategyMap->end())
		{
			OnResponseProcMarco(
				session->getProcessor(),
				MSG_ID_RTN_PRICING, &it->second);
		}
	}

	return std::make_shared<ResultDO>();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateUserParam::HandleResponse(paramvector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateUserParam::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateUserParam::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}