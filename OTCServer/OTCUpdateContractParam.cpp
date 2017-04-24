/***********************************************************************
 * Module:  OTCUpdateContractParam.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:50:23
 * Purpose: Implementation of the class OTCUpdateContractParam
 ***********************************************************************/

#include "OTCUpdateContractParam.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/BizError.h"

#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/ResultDO.h"

#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCUpdateContractParam::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCUpdateContractParam::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCUpdateContractParam::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CheckRolePermission(session, ROLE_TRADINGDESK);

	auto vecConDO_Ptr = (VectorDO<ContractParamDO>*)reqDO.get();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{

		auto mdMap = pWorkerProc->PricingDataContext()->GetMarketDataMap();
		auto contractMap = pWorkerProc->PricingDataContext()->GetContractParamMap();

		for (auto& conDO : *vecConDO_Ptr)
		{
			if (auto pContractDO = contractMap->tryfind(conDO))
			{
				pContractDO->DepthVol = conDO.DepthVol;
				pContractDO->Gamma = conDO.Gamma;
			}

			MarketDataDO mdo;
			if (mdMap->find(conDO.InstrumentID(), mdo))
				pWorkerProc->TriggerUpdateByMarketData(mdo);
		}
	}

	return std::make_shared<ResultDO>();
}
