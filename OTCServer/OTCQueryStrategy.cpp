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

#include "../common/Attribute_Key.h"
#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/IMessageServiceLocator.h"

#include "../dataobject/TemplateDO.h"

#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCQueryStrategy::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of OTCQueryStrategy::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryStrategy::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto sDOVec_Ptr = std::make_shared<VectorDO<StrategyContractDO>>();

	if (auto strategyVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
		session->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
		if (auto wkProcPtr = MessageUtility::ServerWorkerProcessor<OTCWorkerProcessor>(session->getProcessor()))
		{
			auto pStrategyMap = wkProcPtr->GetOTCTradeProcessor()->GetPricingContext()->GetStrategyMap();

			for (auto& strategyKey : *strategyVec_Ptr)
			{
				auto& strategy = pStrategyMap->at(strategyKey);

				if (wkProcPtr)
				{
					int ret = wkProcPtr->RefreshStrategy(strategy);
					wkProcPtr->RegisterPricingListener(strategy,
						session->getProcessor()->LockMessageSession().get());
				}

				sDOVec_Ptr->push_back(strategy);
			}

			ThrowNotFoundException(sDOVec_Ptr);
		}

	return sDOVec_Ptr;
}