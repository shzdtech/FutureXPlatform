/***********************************************************************
 * Module:  CTPOTCSessionProcessor.cpp
 * Author:  milk
 * Modified: 2015年11月6日 22:49:14
 * Purpose: Implementation of the class CTPOTCSessionProcessor
 ***********************************************************************/

#include "CTPOTCSessionProcessor.h"
#include "CTPOTCWorkerProcessor.h"

#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"

#include "../message/message_macro.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCSessionProcessor::CTPOTCSessionProcessor()
 // Purpose:    Implementation of CTPOTCSessionProcessor::CTPOTCSessionProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

CTPOTCSessionProcessor::CTPOTCSessionProcessor()
{
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCSessionProcessor::~CTPOTCSessionProcessor()
// Purpose:    Implementation of CTPOTCSessionProcessor::~CTPOTCSessionProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCSessionProcessor::~CTPOTCSessionProcessor()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCSessionProcessor::Initialize()
// Purpose:    Implementation of CTPOTCSessionProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPOTCSessionProcessor::Initialize(void)
{
	// TODO : implement
}


bool CTPOTCSessionProcessor::OnSessionClosing(void)
{
	if (auto sessionPtr = LockMessageSession())
		if (sessionPtr->getUserInfo()->getRole() == ROLE_TRADINGDESK)
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(shared_from_this()))
			{
				auto pStrategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();

				if (auto strategyVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
					sessionPtr->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
				{
					for (auto& contract : *strategyVec_Ptr)
					{
						auto& strategy = pStrategyMap->at(contract);
						strategy.AskEnabled = strategy.BidEnabled = false;
						if (strategy.Hedging)
						{
							strategy.Hedging = false;
							OrderRequestDO orderDO(strategy);
							pWorkerProc->GetOTCTradeProcessor()->CancelHedgeOrder(orderDO);
						}
					}
				}
			}
	return true;
}