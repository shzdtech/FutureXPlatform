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
// Name:       OTCUpdateContractParam::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of OTCUpdateContractParam::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCUpdateContractParam::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto vecConDO_Ptr = (VectorDO<ContractParamDO>*)reqDO.get();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(session->getProcessor()))
	{

		auto mdMap = pWorkerProc->PricingDataContext()->GetMarketDataMap();
		auto contractMap = pWorkerProc->PricingDataContext()->GetContractParamMap();

		for (auto& conDO : *vecConDO_Ptr)
		{
			auto& contractDO = contractMap->at(conDO);
			contractDO.DepthVol = conDO.DepthVol;
			contractDO.Gamma = conDO.Gamma;

			auto& mdDO = mdMap->at(conDO.InstrumentID());
			pWorkerProc->TriggerUpdating(mdDO);
		}
	}

	return std::make_shared<ResultDO>(serialId);
}
