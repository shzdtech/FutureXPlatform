/***********************************************************************
 * Module:  CTPOTCTradingDeskProcessor.cpp
 * Author:  milk
 * Modified: 2015年11月6日 22:49:14
 * Purpose: Implementation of the class CTPOTCTradingDeskProcessor
 ***********************************************************************/

#include "CTPOTCTradingDeskProcessor.h"
#include "CTPOTCWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"
#include "../CTPServer/Attribute_Key.h"
#include "../CTPServer/CTPAppContext.h"

#include "../pricingengine/PricingContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradingDeskProcessor::CTPOTCTradingDeskProcessor()
// Purpose:    Implementation of CTPOTCTradingDeskProcessor::CTPOTCTradingDeskProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradingDeskProcessor::CTPOTCTradingDeskProcessor(const std::map<std::string, std::string>& configMap)
	: CTPProcessor(configMap)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradingDeskProcessor::~CTPOTCTradingDeskProcessor()
// Purpose:    Implementation of CTPOTCTradingDeskProcessor::~CTPOTCTradingDeskProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPOTCTradingDeskProcessor::~CTPOTCTradingDeskProcessor()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCTradingDeskProcessor::OnInit()
// Purpose:    Implementation of CTPOTCTradingDeskProcessor::OnInit()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPOTCTradingDeskProcessor::OnInit(void)
{
	// TODO : implement
}


bool CTPOTCTradingDeskProcessor::OnSessionClosing(void)
{
	if (auto proc = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(CTPAppContext::FindServerProcessor(WORKPROCESSOR_OTC)))
	{
		auto pStrategyMap = PricingContext::Instance()->GetStrategyMap();
		
		if (auto strategyVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
			getSession()->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
		{
			for (auto& contract : *strategyVec_Ptr)
			{
				auto& strategy = pStrategyMap->at(contract);
				strategy.Enabled = false;
				if (strategy.Trading)
				{
					strategy.Trading = false;
					OrderDO orderDO(strategy);
					proc->CancelAutoOrder(orderDO);
				}
			}
		}
	}

	return true;
}