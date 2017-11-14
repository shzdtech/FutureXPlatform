/***********************************************************************
 * Module:  OTCUnSubTradingDeskData.cpp
 * Author:  milk
 * Modified: 2017年3月19日 12:59:04
 * Purpose: Implementation of the class OTCUnSubTradingDeskData
 ***********************************************************************/

#include "OTCUnSubTradingDeskData.h"
#include "../dataobject/PricingDO.h"
#include "../dataobject/FieldName.h"
#include "../common/Attribute_Key.h"
#include "../message/MessageUtility.h"
#include "../OTCServer/OTCWorkerProcessor.h"

#include "../bizutility/ContractCache.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCUnSubTradingDeskData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
 // Purpose:    Implementation of OTCUnSubTradingDeskData::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCUnSubTradingDeskData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);
	CheckRolePermission(session, UserRoleType::ROLE_TRADINGDESK);

	auto& userInfo = session->getUserInfo();

	auto pInstList = (ContractList*)reqDO.get();
	
	if (auto pWorkerProc = MessageUtility::AbstractWorkerProcessorPtr<OTCWorkerProcessor>(msgProcessor))
	{
		for (auto& inst : *pInstList)
		{
			UserContractKey uck(inst.ExchangeID(), inst.InstrumentID(), userInfo.getUserId());
			pWorkerProc->UnregisterTradingDeskListener(uck, session);
		}
	}

	return reqDO;
}