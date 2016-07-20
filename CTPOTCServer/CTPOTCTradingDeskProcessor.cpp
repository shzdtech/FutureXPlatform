/***********************************************************************
 * Module:  CTPOTCTradingDeskProcessor.cpp
 * Author:  milk
 * Modified: 2015年11月6日 22:49:14
 * Purpose: Implementation of the class CTPOTCTradingDeskProcessor
 ***********************************************************************/

#include "CTPOTCTradingDeskProcessor.h"
#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPWorkerProcessorID.h"
#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"

#include "../message/message_macro.h"

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
// Name:       CTPOTCTradingDeskProcessor::Initialize()
// Purpose:    Implementation of CTPOTCTradingDeskProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPOTCTradingDeskProcessor::Initialize(void)
{
	// TODO : implement
}


bool CTPOTCTradingDeskProcessor::OnSessionClosing(void)
{
	if (auto wkProcPtr =
		MessageUtility::FindGlobalProcessor<CTPOTCWorkerProcessor>(CTPWorkerProcessorID::WORKPROCESSOR_OTC))
	{
		auto pStrategyMap = AttribPointerCast(this, STR_KEY_SERVER_PRICING_DATACONTEXT, IPricingDataContext)
			->GetStrategyMap();

		if (auto sessionptr = LockMessageSession())
		{
			if (auto strategyVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
				sessionptr->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
			{
				for (auto& contract : *strategyVec_Ptr)
				{
					auto& strategy = pStrategyMap->at(contract);
					strategy.Enabled = false;
					if (strategy.Trading)
					{
						strategy.Trading = false;
						OrderDO orderDO(strategy);
						wkProcPtr->CancelAutoOrder(orderDO);
					}
				}
			}
		}
	}
	return true;
}