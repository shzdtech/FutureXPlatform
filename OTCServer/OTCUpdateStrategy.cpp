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
#include "../databaseop/StrategyContractDAO.h"

#include "../bizutility/StrategyModelCache.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCUpdateStrategy::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
 // Purpose:    Implementation of OTCUpdateStrategy::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCUpdateStrategy::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto ret = std::make_shared<VectorDO<StrategyContractDO>>();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto pStrategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();

		auto pStrategyDO = (StrategyContractDO*)reqDO.get();
		pStrategyDO->SetUserID(session->getUserInfo().getUserId());

		StrategyContractDO_Ptr strategy_ptr;
		if (pStrategyMap->find(*pStrategyDO, strategy_ptr))
		{
			if(!pStrategyDO->StrategyName.empty())
				strategy_ptr->StrategyName = pStrategyDO->StrategyName;

			strategy_ptr->BidQV = pStrategyDO->BidQV;
			strategy_ptr->AskQV = pStrategyDO->AskQV;
			strategy_ptr->Depth = pStrategyDO->Depth;

			if (auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>
				(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS)))
			{
				if (auto ucp = userContractMap_Ptr->tryfind(*pStrategyDO))
					ucp->Quantity = strategy_ptr->Quantity;
			}

			strategy_ptr->BidEnabled = pStrategyDO->BidEnabled;
			strategy_ptr->AskEnabled = pStrategyDO->AskEnabled;

			StrategyContractDAO::UpdateStrategy(*strategy_ptr);

			pWorkerProc->TriggerOTCPricing(*strategy_ptr);

			strategy_ptr->Hedging = pStrategyDO->Hedging;

			if (strategy_ptr->Hedging)
				pWorkerProc->GetOTCTradeProcessor()->TriggerHedgeOrderUpdating(*strategy_ptr);
			else
				pWorkerProc->GetOTCTradeProcessor()->CancelHedgeOrder(*strategy_ptr);

			ret->push_back(*strategy_ptr);
		}
	}

	return ret;
}
