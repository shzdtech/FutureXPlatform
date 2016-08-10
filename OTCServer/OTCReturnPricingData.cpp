/***********************************************************************
 * Module:  OTCReturnPricingData.cpp
 * Author:  milk
 * Modified: 2015年8月25日 22:55:51
 * Purpose: Implementation of the class OTCReturnPricingData
 ***********************************************************************/

#include "OTCReturnPricingData.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractParamDO.h"

#include "../message/BizError.h"
#include "../message/MessageUtility.h"
#include "../utility/Encoding.h"

#include "../pricingengine/PricingUtility.h"

#include "../common/Attribute_Key.h"
#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       OTCReturnPricingData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of OTCReturnPricingData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCReturnPricingData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto pStrategyDO = (StrategyContractDO*)rawRespParams[0];

	auto ret = std::make_shared<VectorDO<PricingDO>>();

	if (auto wkProcPtr =
		MessageUtility::ServerWorkerProcessor<OTCWorkerProcessor>(session->getProcessor()))
	{
		auto userContractMap = std::static_pointer_cast<UserContractParamDOMap>
			(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

		for (auto it = userContractMap->begin(); it != userContractMap->end(); it++)
		{
			if (auto pricingDO_ptr = PricingUtility::Pricing(&it->second.Quantity, *pStrategyDO, 
				*wkProcPtr->GetOTCTradeProcessor()->GetPricingContext()))
				ret->push_back(std::move(*pricingDO_ptr));
		}
	}

	return ret;
}