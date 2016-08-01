/***********************************************************************
 * Module:  CTPOTCReturnPricingData.cpp
 * Author:  milk
 * Modified: 2015年8月25日 22:55:51
 * Purpose: Implementation of the class CTPOTCReturnPricingData
 ***********************************************************************/

#include "CTPOTCReturnPricingData.h"

#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractDO.h"

#include "../message/BizError.h"

#include "../utility/Encoding.h"

#include "../pricingengine/PricingUtility.h"

#include "../common/Attribute_Key.h"
#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCReturnPricingData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCReturnPricingData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCReturnPricingData::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto pStrategyDO = (StrategyContractDO*)rawRespParams[0];

	auto ret = std::make_shared<VectorDO<PricingDO>>();

	if (auto pricingCtxPtr = AttribPointerCast(session->getProcessor(), STR_KEY_SERVER_PRICING_DATACONTEXT, IPricingDataContext))
	{
		auto userContractMap = std::static_pointer_cast<UserContractParamDOMap>
			(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

		for (auto it = userContractMap->begin(); it != userContractMap->end(); it++)
		{
			if (auto pricingDO_ptr = PricingUtility::Pricing(&it->second.Quantity, *pStrategyDO, *pricingCtxPtr))
				ret->push_back(std::move(*pricingDO_ptr));
		}
	}

	return ret;
}