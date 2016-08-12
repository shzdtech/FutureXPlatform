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
#include "../message/SysParam.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/FieldName.h"
#include "../databaseop/ContractDAO.h"

#include "../bizutility/ContractCache.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of OTCSubMarketData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto ret = std::make_shared<VectorDO<PricingDO>>();

	auto stdo = (StringTableDO*)reqDO.get();

	if (auto wkProcPtr =
		MessageUtility::ServerWorkerProcessor<OTCWorkerProcessor>(session->getProcessor()))
	{
		if (stdo->Data.size() > 0)
		{
			auto& instList = stdo->Data[STR_INSTRUMENT_ID];

			auto userContractMap_Ptr = std::static_pointer_cast<UserContractParamDOMap>(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

			if (!userContractMap_Ptr)
			{
				userContractMap_Ptr = std::make_shared<UserContractParamDOMap>();
				session->getContext()->setAttribute(STR_KEY_USER_CONTRACTS, userContractMap_Ptr);
			}

			if (session->getUserInfo()->getRole() == ROLE_TRADINGDESK)
			{
				if (auto vectorPtr = ContractDAO::FindContractByClient(session->getUserInfo()->getBrokerId()))
					for (auto& contract : *vectorPtr)
					{
						PricingDO mdo(contract.ExchangeID(), contract.InstrumentID());
						ret->push_back(std::move(mdo));
					}
			}
			else
			{
				for (auto& inst : instList)
				{
					if (auto contract = ContractCache::Get(wkProcPtr->GetProductType()).QueryInstrumentById(inst))
					{
						PricingDO mdo(contract->ExchangeID(), contract->InstrumentID());
						ret->push_back(std::move(mdo));
					}
				}
			}

			auto pSession = session->getProcessor()->LockMessageSession().get();
			for (auto& pDO : *ret)
			{
				wkProcPtr->RegisterPricingListener(pDO, pSession);

				UserContractParamDO ucp(pDO.ExchangeID(), pDO.InstrumentID());
				userContractMap_Ptr->emplace(ucp, std::move(ucp));
			}
		}
	}

	return ret;
}