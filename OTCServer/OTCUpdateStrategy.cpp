/***********************************************************************
 * Module:  OTCUpdateStrategy.cpp
 * Author:  milk
 * Modified: 2015年8月23日 18:51:51
 * Purpose: Implementation of the class OTCUpdateStrategy
 ***********************************************************************/

#include "OTCUpdateStrategy.h"

#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/ResultDO.h"
#include "../dataobject/UserContractParamDO.h"
////////////////////////////////////////////////////////////////////////
// Name:       OTCUpdateStrategy::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of OTCUpdateStrategy::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCUpdateStrategy::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	if (auto wkProcPtr = MessageUtility::ServerWorkerProcessor<OTCWorkerProcessor>(session->getProcessor()))
	{
		auto strategyMap = wkProcPtr->GetOTCTradeProcessor()->GetPricingContext()->GetStrategyMap();

		auto strategyVec = (VectorDO<StrategyContractDO>*)reqDO.get();

		if (auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>
			(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS)))
		{

			for (auto& strategyDO : *strategyVec)
			{
				auto it = strategyMap->find(strategyDO);
				if (it != strategyMap->end())
				{
					StrategyContractDO& scDO = it->second;
					scDO.Quantity = strategyDO.Quantity;
					scDO.Depth = strategyDO.Depth;
					auto uit = userContractMap_Ptr->find(strategyDO);
					if (uit != userContractMap_Ptr->end())
						uit->second.Quantity = scDO.Quantity;

					scDO.Enabled = strategyDO.Enabled;
					scDO.Volatility = scDO.Volatility;
					for (auto& pair : strategyDO.Params)
						scDO.Params[pair.first] = pair.second;

					if (scDO.Enabled)
						wkProcPtr->TriggerPricing(scDO);
					else
						strategyDO.Trading = false;

					scDO.Trading = strategyDO.Trading;

					if (scDO.Trading)
						wkProcPtr->GetOTCTradeProcessor()->TriggerHedgeOrderUpdating(scDO);
					else
						wkProcPtr->GetOTCTradeProcessor()->CancelHedgeOrder(scDO);
				}
			}
		}
	}

	return reqDO;
}
