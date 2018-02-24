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

#include "../bizutility/ModelParamsCache.h"

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
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	StrategyContractDO_Ptr strategy_ptr;

	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto pStrategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();

		auto pStrategyDO = (StrategyContractDO*)reqDO.get();
		pStrategyDO->SetUserID(session->getUserInfo().getUserId());

		if (pStrategyMap->find(*pStrategyDO, strategy_ptr))
		{
			if (!pStrategyDO->StrategyName.empty())
				strategy_ptr->StrategyName = pStrategyDO->StrategyName;

			strategy_ptr->Depth = pStrategyDO->Depth;
			strategy_ptr->Multiplier = pStrategyDO->Multiplier;
			strategy_ptr->AutoOrderEnabled = pStrategyDO->AutoOrderEnabled;

			strategy_ptr->AutoOrderSettings.BidQV = pStrategyDO->AutoOrderSettings.BidQV;
			strategy_ptr->AutoOrderSettings.AskQV = pStrategyDO->AutoOrderSettings.AskQV;
			strategy_ptr->AutoOrderSettings.MaxAutoTrade = pStrategyDO->AutoOrderSettings.MaxAutoTrade;
			strategy_ptr->AutoOrderSettings.CloseMode = pStrategyDO->AutoOrderSettings.CloseMode;
			strategy_ptr->AutoOrderSettings.VolCondition = pStrategyDO->AutoOrderSettings.VolCondition;
			
			int limitOrderCnt = pWorkerProc->GetOTCTradeWorkerProcessor()->GetExchangeOrderContext().GetLimitOrderCount(strategy_ptr->InstrumentID());
			if (strategy_ptr->AutoOrderSettings.LimitOrderCounter < limitOrderCnt)
				strategy_ptr->AutoOrderSettings.LimitOrderCounter = limitOrderCnt;

			if (pStrategyDO->AutoOrderSettings.AskCounter < 0)
				strategy_ptr->AutoOrderSettings.AskCounter = 0;

			if (pStrategyDO->AutoOrderSettings.BidCounter < 0)
				strategy_ptr->AutoOrderSettings.BidCounter = 0;

			if (auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>
				(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS)))
			{
				if (auto ucp = userContractMap_Ptr->tryfind(*pStrategyDO))
					ucp->Quantity = strategy_ptr->Quantity;
			}

			if (strategy_ptr->BidEnabled != pStrategyDO->BidEnabled ||
				strategy_ptr->AskEnabled != pStrategyDO->AskEnabled)
			{
				strategy_ptr->BidEnabled = pStrategyDO->BidEnabled;
				strategy_ptr->AskEnabled = pStrategyDO->AskEnabled;
				StrategyContractDAO::UpdateStrategy(*strategy_ptr);
			}

			if (strategy_ptr->TickSizeMult != pStrategyDO->TickSizeMult ||
				strategy_ptr->NotCross != pStrategyDO->NotCross ||
				strategy_ptr->AutoOrderSettings.TIF != pStrategyDO->AutoOrderSettings.TIF)
			{
				strategy_ptr->TickSizeMult = pStrategyDO->TickSizeMult;
				strategy_ptr->NotCross = pStrategyDO->NotCross;
				strategy_ptr->AutoOrderSettings.TIF = pStrategyDO->AutoOrderSettings.TIF;
				pWorkerProc->TriggerPricingByStrategy(*strategy_ptr);
			}
			

			if (strategy_ptr->AutoOrderEnabled)
			{
				if (CheckAllowTrade(session, false))
				{
					pWorkerProc->GetOTCTradeWorkerProcessor()->TriggerAutoOrderUpdating(*strategy_ptr);
				}
				else
				{
					strategy_ptr->AutoOrderEnabled = false;
				}
			}
			else
			{
				pWorkerProc->GetOTCTradeWorkerProcessor()->CancelAutoOrder(*strategy_ptr);
			}
		}
	}

	return strategy_ptr;
}

dataobj_ptr OTCUpdateStrategy::HandleResponse(const uint32_t serialId, const param_vector & rawRespParams, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	return *(StrategyContractDO_Ptr*)rawRespParams[0];
}
