/***********************************************************************
 * Module:  OTCSubMarketData.cpp
 * Author:  milk
 * Modified: 2015年8月2日 14:14:39
 * Purpose: Implementation of the class OTCSubMarketData
 ***********************************************************************/

#include "OTCOptionSubMarketData.h"
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
 // Name:       OTCSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of OTCSubMarketData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCOptionSubMarketData::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto ret = std::make_shared<VectorDO<PricingDO>>();

	auto stdo = (StringTableDO*)reqDO.get();

	if (auto wkProcPtr =
		MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(session->getProcessor()))
	{
		if (!stdo->Data.empty())
		{
			auto& instList = stdo->Data.begin()->second;
			auto pSession = session->getProcessor()->LockMessageSession().get();

			if (session->getUserInfo()->getRole() == ROLE_TRADINGDESK)
			{
				if (auto strategyVec_Ptr = std::static_pointer_cast<std::vector<ContractKey>>(
					pSession->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
				{
					for (auto& inst : instList)
					{
						if (auto pContract = wkProcPtr->GetInstrumentCache().QueryInstrumentById(inst))
						{
							if (std::find(strategyVec_Ptr->begin(), strategyVec_Ptr->end(), *pContract) != strategyVec_Ptr->end())
							{
								PricingDO mdo(pContract->ExchangeID(), pContract->InstrumentID());
								wkProcPtr->RegisterTradingDeskListener(mdo, pSession);
								ret->push_back(std::move(mdo));
							}
						}
					}
				}
			}
			else
			{
				for (auto& inst : instList)
				{
					if (auto pContract = wkProcPtr->GetInstrumentCache().QueryInstrumentById(inst))
					{
						PricingDO mdo(pContract->ExchangeID(), pContract->InstrumentID());
						wkProcPtr->RegisterPricingListener(mdo, pSession);
						ret->push_back(std::move(mdo));
					}
				}
			}
		}
	}

	return ret;
}