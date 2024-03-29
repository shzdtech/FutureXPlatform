/***********************************************************************
 * Module:  OTCSubMarketData.cpp
 * Author:  milk
 * Modified: 2015年8月2日 14:14:39
 * Purpose: Implementation of the class OTCSubMarketData
 ***********************************************************************/

#include "OTCSubMarketData.h"
#include "../common/BizErrorIDs.h"

#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include <set>

#include "../message/BizError.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/FieldName.h"
#include "../databaseop/ContractDAO.h"

#include "../bizutility/ContractCache.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCSubMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCSubMarketData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCSubMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto ret = std::make_shared<VectorDO<PricingDO>>();

	auto stdo = (VectorDO<ContractKey>*)reqDO.get();

	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		if (!stdo->empty())
		{
			auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

			if (!userContractMap_Ptr)
			{
				userContractMap_Ptr = std::make_shared<UserContractParamDOMap>();
				session->getContext()->setAttribute(STR_KEY_USER_CONTRACTS, userContractMap_Ptr);
			}

			auto pricingDOMap = pWorkerProc->PricingDataContext()->GetPricingDataDOMap();

			for (auto& inst : *stdo)
			{
				PricingDO mdo(inst.ExchangeID(), inst.InstrumentID());

				IPricingDO_Ptr pricing_ptr;
				if (pricingDOMap->find(mdo, pricing_ptr))
				{
					mdo.Ask() = pricing_ptr->Ask();
					mdo.Bid() = pricing_ptr->Bid();
				}

				ret->push_back(std::move(mdo));
			}

			auto sessionPtr = msgProcessor->getMessageSession();
			for (auto& pDO : *ret)
			{
				pWorkerProc->RegisterPricingListener(pDO, sessionPtr);

				UserContractParamDO ucp(pDO.ExchangeID(), pDO.InstrumentID());
				userContractMap_Ptr->emplace(ucp, std::move(ucp));
			}
		}
	}

	return ret;
}