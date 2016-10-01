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

	auto ret = std::make_shared<VectorDO<StrategyContractDO>>();

	if (auto wkProcPtr = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(session->getProcessor()))
	{
		auto strategyMap = wkProcPtr->PricingDataContext()->GetStrategyMap();

		auto strategyDO = *(StrategyContractDO*)reqDO.get();
		strategyDO.SetUserID(session->getUserInfo()->getUserId());

		auto it = strategyMap->find(strategyDO);
		if (it != strategyMap->end())
		{
			StrategyContractDO& scDO = it->second;
			if(!strategyDO.StrategyName.empty())
				scDO.StrategyName = strategyDO.StrategyName;

			scDO.BidQT = strategyDO.BidQT;
			scDO.AskQT = strategyDO.AskQT;
			scDO.Depth = strategyDO.Depth;

			if (auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>
				(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS)))
			{
				if (auto ucp = userContractMap_Ptr->tryfind(strategyDO))
					ucp->Quantity = scDO.BidQT;
			}

			scDO.BidEnabled = strategyDO.BidEnabled;
			scDO.AskEnabled = strategyDO.AskEnabled;

			if (auto cnt = strategyDO.PricingContracts.size())
			{
				if(scDO.PricingContracts.size() != cnt)
				{
					scDO.PricingContracts.resize(cnt);
				}

				for (int i = 0; i < cnt; i++)
				{
					scDO.PricingContracts[i] = strategyDO.PricingContracts[i];
				}
			}

			if (strategyDO.PricingModel && scDO.PricingModel->InstanceName != strategyDO.PricingModel->InstanceName)
			{
				if (auto model = StrategyModelCache::FindOrCreateModel(*strategyDO.PricingModel))
					scDO.PricingModel = model;
			}

			if (strategyDO.IVModel && scDO.IVModel->InstanceName != strategyDO.IVModel->InstanceName)
			{
				if (auto model = StrategyModelCache::FindOrCreateModel(*strategyDO.IVModel))
					scDO.IVModel = model;
			}

			if (strategyDO.VolModel && scDO.VolModel->InstanceName != strategyDO.VolModel->InstanceName)
			{
				if (auto model = StrategyModelCache::FindOrCreateModel(*strategyDO.VolModel))
					scDO.VolModel = model;
			}

			StrategyContractDAO::UpdateStrategy(scDO);

			wkProcPtr->TriggerOTCPricing(scDO);

			scDO.Hedging = strategyDO.Hedging;

			if (scDO.Hedging)
				wkProcPtr->GetOTCTradeProcessor()->TriggerHedgeOrderUpdating(scDO);
			else
				wkProcPtr->GetOTCTradeProcessor()->CancelHedgeOrder(scDO);

			ret->push_back(scDO);
		}
	}

	return ret;
}
