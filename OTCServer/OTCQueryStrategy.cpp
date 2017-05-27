/***********************************************************************
 * Module:  OTCQueryStrategy.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:46:02
 * Purpose: Implementation of the class OTCQueryStrategy
 ***********************************************************************/

#include "OTCQueryStrategy.h"

#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/MessageUtility.h"

#include "../common/Attribute_Key.h"
#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/IMessageServiceLocator.h"

#include "../dataobject/TemplateDO.h"

#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCQueryStrategy::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCQueryStrategy::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryStrategy::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	auto sDOVec_Ptr = std::make_shared<VectorDO<StrategyContractDO>>();

	if (auto strategySet_Ptr = std::static_pointer_cast<std::set<UserContractKey>>(
		session->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
		{
			auto& userOrderCtx = pWorkerProc->GetOTCTradeProcessor()->GetExchangeOrderContext();
			auto pStrategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();
			auto sessionPtr = msgProcessor->getMessageSession();

			for (auto& strategyKey : *strategySet_Ptr)
			{
				StrategyContractDO_Ptr strategy_ptr;
				if (pStrategyMap->find(strategyKey, strategy_ptr))
				{
					int orderCnt = userOrderCtx.GetLimitOrderCount(strategyKey.InstrumentID());
					if (strategy_ptr->AutoOrderSettings.LimitOrderCounter < orderCnt)
						strategy_ptr->AutoOrderSettings.LimitOrderCounter = orderCnt;

					sDOVec_Ptr->push_back(*strategy_ptr);
				}
			}

		}

	return sDOVec_Ptr;
}