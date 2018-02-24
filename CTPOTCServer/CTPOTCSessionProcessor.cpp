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
	try
	{
		if (auto sessionPtr = getMessageSession())
			if (sessionPtr->getUserInfo().getRole() == ROLE_TRADINGDESK)
				if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(this))
				{
					if (auto pWorkerTrader = pWorkerProc->GetOTCTradeWorkerProcessor())
					{
						// Dispose auto order
						auto pUserStrategyMap = pWorkerProc->PricingDataContext()->GetUserStrategyMap();
						auto it = pUserStrategyMap->find(sessionPtr->getUserInfo().getUserId());
						if (it != pUserStrategyMap->end())
						{
							for (auto& strategyMap : it->second)
							{
								for (auto& pair : strategyMap.second->lock_table())
								{
									if (auto strategy_ptr = pair.second)
									{
										strategy_ptr->AskEnabled = strategy_ptr->BidEnabled = false;
										strategy_ptr->AutoOrderEnabled = false;
										pWorkerTrader->CancelAutoOrder(*strategy_ptr);
									}
								}
							}
						}

						// Dispose hedge order

						if (auto pPortfolioMap = pWorkerProc->PricingDataContext()->GetPortfolioMap()
							->tryfind(sessionPtr->getUserInfo().getUserId()))
						{
							for (auto& pair : *pPortfolioMap)
							{
								pair.second.Hedging = false;
								pWorkerTrader->CancelHedgeOrder(pair.second);
							}
						}
					}
				}
	}
	catch (std::exception& e)
	{
		LOG_ERROR << e.what();
	}

	return true;
}