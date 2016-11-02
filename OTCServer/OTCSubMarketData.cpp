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
 // Name:       OTCSubMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of OTCSubMarketData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCSubMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto ret = std::make_shared<VectorDO<PricingDO>>();

	auto stdo = (StringTableDO*)reqDO.get();

	if (auto pWorkerProc =
		MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(session->getProcessor()))
	{
		if (!stdo->Data.empty())
		{
			auto& instList = stdo->Data.begin()->second;

			auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

			if (!userContractMap_Ptr)
			{
				userContractMap_Ptr = std::make_shared<UserContractParamDOMap>();
				session->getContext()->setAttribute(STR_KEY_USER_CONTRACTS, userContractMap_Ptr);
			}

			if (session->getUserInfo()->getRole() == ROLE_TRADINGDESK)
			{
				if (auto strategyVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
					session->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
				{
					for (auto& contract : *strategyVec_Ptr)
					{
						PricingDO mdo(contract.ExchangeID(), contract.InstrumentID());
						ret->push_back(std::move(mdo));
					}
				}
			}
			else
			{
				for (auto& inst : instList)
				{
					if (auto contract = pWorkerProc->GetInstrumentCache().QueryInstrumentById(inst))
					{
						PricingDO mdo(contract->ExchangeID(), contract->InstrumentID());
						ret->push_back(std::move(mdo));
					}
				}
			}

			auto sessionPtr = session->getProcessor()->LockMessageSession();
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