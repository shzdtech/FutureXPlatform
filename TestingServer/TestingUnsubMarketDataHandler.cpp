/***********************************************************************
 * Module:  TestingUnsubMarketDataHandler.cpp
 * Author:  milk
 * Modified: 2016年4月29日 19:25:47
 * Purpose: Implementation of the class TestingUnsubMarketDataHandler
 ***********************************************************************/

#include "TestingUnsubMarketDataHandler.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/TypedefDO.h"
#include "../common/Attribute_Key.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       TestingUnsubMarketDataHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
 // Purpose:    Implementation of TestingUnsubMarketDataHandler::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingUnsubMarketDataHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (auto mdMapPtr = std::static_pointer_cast<MarketDataDOMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS)))
	{
		auto stdo = (StringTableDO*)reqDO.get();

		if (!stdo->Data.empty())
		{
			auto& instList = stdo->Data[STR_INSTRUMENT_ID];
			for (auto& inst : instList)
			{
				mdMapPtr->erase(inst);
			}
		}
	}
	return reqDO;
}