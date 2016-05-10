/***********************************************************************
 * Module:  CTPOTCReturnPricingData.cpp
 * Author:  milk
 * Modified: 2015年8月25日 22:55:51
 * Purpose: Implementation of the class CTPOTCReturnPricingData
 ***********************************************************************/

#include "CTPOTCReturnPricingData.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/Attribute_Key.h"
#include "../pricingengine/PricingContext.h"
#include <glog/logging.h>

#include "../dataobject/MarketDataDO.h"
#include "../dataobject/UserContractParam.h"
#include "../dataobject/StrategyContractDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/ContractDO.h"

#include "../message/BizError.h"

#include "../utility/Encoding.h"

#include "../pricingengine/PricingUtility.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCReturnPricingData::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCReturnPricingData::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCReturnPricingData::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto pStrategyDO = (StrategyContractDO*)rawRespParams[0];

	auto ret = std::make_shared<VectorDO<PricingDO>>();

	auto userContractMap = std::static_pointer_cast<UserContractParamMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

	for (auto it = userContractMap->begin(); it != userContractMap->end(); it++)
	{
		if (auto pricingDO_ptr = PricingUtility::Pricing(*pStrategyDO, it->second.Quantity))
			ret->push_back(std::move(*pricingDO_ptr));
	}

	return ret;
}