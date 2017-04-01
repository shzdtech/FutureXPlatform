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

#include "../litelogger/LiteLogger.h"

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
	LOG_DEBUG << __FUNCTION__;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCSessionProcessor::Initialize()
// Purpose:    Implementation of CTPOTCSessionProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPOTCSessionProcessor::Initialize(IServerContext* serverCtx)
{
	// TODO : implement
}


bool CTPOTCSessionProcessor::OnSessionClosing(void)
{
	/*if (auto sessionPtr = getMessageSession())
		if (sessionPtr->getUserInfo().getRole() == ROLE_TRADINGDESK)
			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(shared_from_this()))
			{
				auto pStrategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();

				if (auto strategySet_Ptr = std::static_pointer_cast<std::set<ContractKey>>(
					sessionPtr->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
				{
					for (auto& contract : *strategySet_Ptr)
					{
						if (auto pStrategy = pStrategyMap->tryfind(contract))
						{
							pStrategy->AskEnabled = pStrategy->BidEnabled = false;
							if (pStrategy->Hedging)
							{
								pStrategy->Hedging = false;
								OrderRequestDO orderDO(*pStrategy);
								pWorkerProc->GetOTCTradeProcessor()->CancelHedgeOrder(orderDO);
							}
						}
					}
				}
			}*/
	return true;
}