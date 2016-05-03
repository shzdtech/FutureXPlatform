/***********************************************************************
 * Module:  CTPOTCUpdateStrategy.cpp
 * Author:  milk
 * Modified: 2015年8月23日 18:51:51
 * Purpose: Implementation of the class CTPOTCUpdateStrategy
 ***********************************************************************/

#include "CTPOTCUpdateStrategy.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/Attribute_Key.h"
#include "../CTPServer/CTPAppContext.h"
#include "../strategy/PricingContext.h"
#include "CTPOTCWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../message/BizError.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/ResultDO.h"
#include "../dataobject/UserContractParam.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateStrategy::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateStrategy::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateStrategy::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckLogin(session);

	auto strategyMap = PricingContext::GetStrategyMap();

	auto strategyVec = (VectorDO<StrategyContractDO>*)reqDO.get();

	auto proc = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(CTPAppContext::FindServerProcessor(WORKPROCESSOR_OTC));

	auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

	for (auto& strategyDO : *strategyVec)
	{
		auto it = strategyMap->find(strategyDO);
		if (it != strategyMap->end())
		{
			StrategyContractDO& scDO = it->second;
			scDO.Offset = strategyDO.Offset;
			scDO.Quantity = strategyDO.Quantity;
			scDO.Depth = strategyDO.Depth;
			scDO.Spread = strategyDO.Spread;
			userContractMap_Ptr->at(strategyDO).Quantity = scDO.Quantity;
			scDO.Enabled = strategyDO.Enabled;

			if (scDO.Enabled)
				proc->TriggerPricing(scDO);
			else
				strategyDO.Trading = false;

			scDO.Trading = strategyDO.Trading;

			if (scDO.Trading)
				proc->TriggerOrderUpdating(scDO);
			else
				proc->CancelAutoOrder(scDO);
		}
	}

	return reqDO;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateStrategy::HandleResponse(ParamVector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateStrategy::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateStrategy::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}