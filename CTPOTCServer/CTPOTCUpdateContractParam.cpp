/***********************************************************************
 * Module:  CTPOTCUpdateContractParam.cpp
 * Author:  milk
 * Modified: 2015年8月22日 15:50:23
 * Purpose: Implementation of the class CTPOTCUpdateContractParam
 ***********************************************************************/

#include "CTPOTCUpdateContractParam.h"
#include "CTPOTCWorkerProcessor.h"
#include "../CTPServer/CTPWorkerProcessorID.h"


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
// Name:       CTPOTCUpdateContractParam::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateContractParam::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateContractParam::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto vecConDO_Ptr = (VectorDO<ContractDO>*)reqDO.get();

	if (auto pricingCtxPtr = AttribPointerCast(session->getProcessor(),
		STR_KEY_SERVER_PRICING_DATACONTEXT, IPricingDataContext))
	{

		auto mdMap = pricingCtxPtr->GetMarketDataDOMap();
		auto contractMap = pricingCtxPtr->GetContractMap();

		auto wkProcPtr =
			MessageUtility::FindGlobalProcessor<CTPOTCWorkerProcessor>(CTPWorkerProcessorID::WORKPROCESSOR_OTC);

		for (auto& conDO : *vecConDO_Ptr)
		{
			auto& contractDO = contractMap->at(conDO);
			contractDO.DepthVol = conDO.DepthVol;
			contractDO.Gamma = conDO.Gamma;

			auto& mdDO = mdMap->at(conDO.InstrumentID());
			wkProcPtr->TriggerUpdating(mdDO);
		}
	}

	return std::make_shared<ResultDO>(reqDO->SerialId);
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCUpdateContractParam::HandleResponse(const uint32_t serialId, paramvector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCUpdateContractParam::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCUpdateContractParam::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}