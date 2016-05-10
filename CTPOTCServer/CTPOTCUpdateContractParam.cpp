/***********************************************************************
 * Module:  CTPOTCUpdateContractParam.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:50:23
 * Purpose: Implementation of the class CTPOTCUpdateContractParam
 ***********************************************************************/

#include "CTPOTCUpdateContractParam.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/CTPAppContext.h"
#include <glog/logging.h>
#include "../pricingengine/PricingContext.h"
#include "CTPOTCWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../message/BizError.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/ResultDO.h"

#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateContractParam::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateContractParam::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateContractParam::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckLogin(session);

	auto vecConDO_Ptr = (VectorDO<ContractDO>*)reqDO.get();
	auto mdMap = PricingContext::Instance()->GetMarketDataDOMap();
	auto contractMap = PricingContext::Instance()->GetContractMap();

	auto proc = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(CTPAppContext::FindServerProcessor(WORKPROCESSOR_OTC));

	for (auto& conDO : *vecConDO_Ptr)
	{
		auto& contractDO = contractMap->at(conDO);
		contractDO.DepthVol = conDO.DepthVol;
		contractDO.Gamma = conDO.Gamma;

		auto& mdDO = mdMap->at(conDO.InstrumentID());
		proc->TriggerUpdating(mdDO);
	}	

	return std::make_shared<ResultDO>();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateContractParam::HandleResponse(paramvector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateContractParam::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateContractParam::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}