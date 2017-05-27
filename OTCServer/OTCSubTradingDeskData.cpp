/***********************************************************************
 * Module:  OTCSubTradingDeskData.cpp
 * Author:  milk
 * Modified: 2017年3月17日 14:14:39
 * Purpose: Implementation of the class OTCSubTradingDeskData
 ***********************************************************************/

#include "OTCSubTradingDeskData.h"
#include "../common/BizErrorIDs.h"

#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../message/BizError.h"

#include "../OTCServer/OTCWorkerProcessor.h"


#include "../dataobject/TemplateDO.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/UserContractParamDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/FieldName.h"
#include "../databaseop/ContractDAO.h"

#include "../bizutility/ContractCache.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCSubTradingDeskData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCSubTradingDeskData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCSubTradingDeskData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	auto pInstList = (ContractList*)reqDO.get();
	
	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		auto& userInfo = session->getUserInfo();

		auto ret = std::make_shared<ContractList>();
		if (auto strategySet_Ptr = std::static_pointer_cast<std::set<UserContractKey>>(
			session->getContext()->getAttribute(STR_KEY_USER_STRATEGY)))
		{
			auto pStrategyMap = pWorkerProc->PricingDataContext()->GetStrategyMap();
			for (auto& inst : *pInstList)
			{
				UserContractKey uck(inst.ExchangeID(), inst.InstrumentID(), userInfo.getUserId());
				if (strategySet_Ptr->find(uck) != strategySet_Ptr->end())
				{
					pWorkerProc->RegisterTradingDeskListener(inst, session);
					ret->push_back(inst);
				}
			}

			OnResponseProcMacro(msgProcessor, MSG_ID_SUB_TRADINGDESK_PRICING, reqDO->SerialId, &ret);

			// std::this_thread::sleep_for(std::chrono::seconds(1));
			for (auto& contract : *ret)
			{
				UserContractKey uck(contract.ExchangeID(), contract.InstrumentID(), userInfo.getUserId());
				StrategyContractDO_Ptr strategy_ptr;
				if (pStrategyMap->find(uck, strategy_ptr))
				{
					pWorkerProc->TriggerTadingDeskParams(*strategy_ptr);
				}
			}
		}
	}

	return nullptr;
}

dataobj_ptr OTCSubTradingDeskData::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return *(std::shared_ptr<ContractList>*)rawRespParams[0];
}