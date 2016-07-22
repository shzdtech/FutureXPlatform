/***********************************************************************
 * Module:  CTPOTCUpdateStrategy.cpp
 * Author:  milk
 * Modified: 2015年8月23日 18:51:51
 * Purpose: Implementation of the class CTPOTCUpdateStrategy
 ***********************************************************************/

#include "CTPOTCUpdateStrategy.h"

#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPWorkerProcessorID.h"

#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/ResultDO.h"
#include "../dataobject/UserContractParamDO.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateStrategy::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateStrategy::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateStrategy::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto strategyMap = AttribPointerCast(session->getProcessor(), 
		STR_KEY_SERVER_PRICING_DATACONTEXT, IPricingDataContext)->GetStrategyMap();

	auto strategyVec = (VectorDO<StrategyContractDO>*)reqDO.get();

	auto wkProcPtr =
		MessageUtility::FindGlobalProcessor<CTPOTCWorkerProcessor>(CTPWorkerProcessorID::WORKPROCESSOR_OTC);

	auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>
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
			scDO.RiskFreeRate = scDO.RiskFreeRate;
			scDO.Strike = scDO.Strike;
			scDO.Volatility = scDO.Volatility;

			if (scDO.Enabled)
				wkProcPtr->TriggerPricing(scDO);
			else
				strategyDO.Trading = false;

			scDO.Trading = strategyDO.Trading;

			if (scDO.Trading)
				wkProcPtr->TriggerOrderUpdating(scDO);
			else
				wkProcPtr->CancelAutoOrder(scDO);
		}
	}

	return reqDO;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateStrategy::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateStrategy::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateStrategy::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}