/***********************************************************************
 * Module:  TestingSubMarketDataHandler.cpp
 * Author:  milk
 * Modified: 2016年4月29日 19:24:42
 * Purpose: Implementation of the class TestingSubMarketDataHandler
 ***********************************************************************/

#include "TestingSubMarketDataHandler.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/TypedefDO.h"
#include "../CTPServer/Attribute_Key.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       TestingSubMarketDataHandler::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of TestingSubMarketDataHandler::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingSubMarketDataHandler::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();

	std::shared_ptr<MarketDataDOMap> mdDOMap_Ptr =
		std::static_pointer_cast<MarketDataDOMap>
		(session->getContext()->getAttribute(STR_KEY_USER_CONTRACTS));

	if (!mdDOMap_Ptr)
	{
		mdDOMap_Ptr =
			std::make_shared<MarketDataDOMap>();
		session->getContext()->setAttribute(STR_KEY_USER_CONTRACTS, mdDOMap_Ptr);
	}

	if (stdo->Data.size() > 0)
	{
		auto& instList = stdo->Data[STR_INSTRUMENT_ID];
		for (auto& inst : instList)
		{
			MarketDataDO mdDO("TestExchange", inst);
			mdDOMap_Ptr->emplace(inst, std::move(mdDO));
		}
	}

	return nullptr;
}