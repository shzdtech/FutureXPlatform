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
 // Name:       OTCSubMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCSubMarketData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCOptionSubMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto ret = std::make_shared<VectorDO<PricingDO>>();

	auto stdo = (VectorDO<ContractKey>*)reqDO.get();

	if (auto pWorkerProc =
		MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto& userInfo = session->getUserInfo();

		if (!stdo->empty())
		{
			auto sessionPtr = msgProcessor->getMessageSession();

			if (session->getUserInfo().getRole() == ROLE_TRADINGDESK)
			{
				auto pStrategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();
				for (auto& inst : *stdo)
				{
					if (inst.ExchangeID().empty())
					{
						if (auto pContract = pWorkerProc->GetInstrumentCache().QueryInstrumentById(inst.InstrumentID()))
							inst.setExchangeID(pContract->ExchangeID());
					}

					UserContractKey uck(inst.ExchangeID(), inst.InstrumentID(), userInfo.getUserId());
					if (pStrategyMap->contains(uck))
					{
						PricingDO mdo(inst.ExchangeID(), inst.InstrumentID());
						ret->push_back(std::move(mdo));
					}
				}
			}
			else
			{
				for (auto& inst : *stdo)
				{
					if (inst.ExchangeID().empty())
					{
						if (auto pContract = pWorkerProc->GetInstrumentCache().QueryInstrumentById(inst.InstrumentID()))
							inst.setExchangeID(pContract->ExchangeID());
					}

					PricingDO mdo(inst.ExchangeID(), inst.InstrumentID());
					pWorkerProc->RegisterPricingListener(mdo, sessionPtr);
					ret->push_back(std::move(mdo));
				}
			}
		}
	}

	return ret;
}