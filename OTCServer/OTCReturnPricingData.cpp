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
#include <boost/locale/encoding.hpp>

#include "../pricingengine/PricingUtility.h"

#include "../common/Attribute_Key.h"
#include "../message/message_macro.h"
#include "../pricingengine/IPricingDataContext.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCReturnPricingData::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCReturnPricingData::HandleResponse(const uint32_t serialId, )
 // Parameters:
 // - rawRespParams
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCReturnPricingData::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto pStrategyDO = (StrategyContractDO*)rawRespParams[1];

	IPricingDO_Ptr ret;

	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto userContractMap = std::static_pointer_cast<UserContractParamDOMap>
			(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

		if (auto pContractParam = userContractMap->tryfind(*pStrategyDO))
		{
			int quantity = pContractParam->Quantity;

			ret = PricingUtility::Pricing(&quantity, *pStrategyDO, pWorkerProc->PricingDataContext());
			if (!pStrategyDO->BidEnabled)
			{
				ret->Bid().Clear();
			}

			if (!pStrategyDO->AskEnabled)
			{
				ret->Ask().Clear();
			}
		}
	}

	return ret;
}