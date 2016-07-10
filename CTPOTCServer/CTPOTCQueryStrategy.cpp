/***********************************************************************
 * Module:  CTPOTCQueryStrategy.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:46:02
 * Purpose: Implementation of the class CTPOTCQueryStrategy
 ***********************************************************************/

#include "CTPOTCQueryStrategy.h"


#include "../CTPServer/CTPWorkerProcessorID.h"
#include "CTPOTCWorkerProcessor.h"

#include "../message/GlobalProcessorRegistry.h"
#include "../common/Attribute_Key.h"

#include "../CTPServer/CTPUtility.h"
#include "../common/Attribute_Key.h"
#include "../message/BizError.h"
#include "../message/message_macro.h"
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
	CheckLogin(session);

	auto strategyVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
		session->getContext()->getAttribute(STR_KEY_USER_STRATEGY));

	auto sDOVec_Ptr = std::make_shared<VectorDO<StrategyContractDO>>();
	sDOVec_Ptr->SerialId = reqDO->SerialId;

	auto wkProcPtr = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::WORKPROCESSOR_OTC));

	auto pricingCtx = AttribPointerCast(session->getProcessor(),
		STR_KEY_SERVER_PRICING_DATACONTEXT, IPricingDataContext);

	auto pStrategyMap = pricingCtx->GetStrategyMap();

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
// Name:       CTPOTCQueryStrategy::HandleResponse(const uint32_t serialId, paramvector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryStrategy::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryStrategy::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}