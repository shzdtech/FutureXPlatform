/***********************************************************************
 * Module:  CTPOTCQueryStrategy.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:46:02
 * Purpose: Implementation of the class CTPOTCQueryStrategy
 ***********************************************************************/

#include "CTPOTCQueryStrategy.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/CTPAppContext.h"
#include "../CTPServer/Attribute_Key.h"
#include "../pricingengine/PricingContext.h"
#include "CTPWorkerProcessorID.h"
#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPRawAPI.h"

#include <glog/logging.h>

#include "../message/BizError.h"
#include "../message/IMessageServiceLocator.h"

#include "../dataobject/TemplateDO.h"

#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"


////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryStrategy::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryStrategy::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryStrategy::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckLogin(session);

	auto strategyVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
		session->getContext()->getAttribute(STR_KEY_USER_STRATEGY));

	auto sDOVec_Ptr = std::make_shared<VectorDO<StrategyContractDO>>();

	auto wkProcPtr = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(CTPAppContext::FindServerProcessor(CTPWorkProcessorID::WORKPROCESSOR_OTC));

	auto pStrategyMap = PricingContext::Instance()->GetStrategyMap();

	for (auto& strategyKey : *strategyVec_Ptr)
	{
		auto& strategy = pStrategyMap->at(strategyKey);

		if (wkProcPtr)
		{
			int ret = wkProcPtr->RefreshStrategy(strategy);
			CTPUtility::HasReturnError(ret);
			wkProcPtr->RegisterPricingListener(strategy, (IMessageSession*)session);
		}

		sDOVec_Ptr->push_back(strategy);
	}


	return sDOVec_Ptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryStrategy::HandleResponse(paramvector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryStrategy::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryStrategy::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}