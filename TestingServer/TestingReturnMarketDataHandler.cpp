/***********************************************************************
 * Module:  TestingReturnMarketDataHandler.cpp
 * Author:  milk
 * Modified: 2016年4月29日 23:13:22
 * Purpose: Implementation of the class TestingReturnMarketDataHandler
 ***********************************************************************/

#include "TestingReturnMarketDataHandler.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/TemplateDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       TestingReturnMarketDataHandler::HandleResponse(const uint32_t serialId, const uint32_t serialId, param_vector rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of TestingReturnMarketDataHandler::HandleResponse(const uint32_t serialId, const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr TestingReturnMarketDataHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	return reqDO;
}

dataobj_ptr TestingReturnMarketDataHandler::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	return std::make_shared<MarketDataDO>(*((MarketDataDO*)rawParams[0]));
}