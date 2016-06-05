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

dataobj_ptr TestingReturnMarketDataHandler::HandleResponse(const uint32_t serialId, param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	auto pDOVec = new VectorDO<MarketDataDO>;
	dataobj_ptr ret(pDOVec);

	pDOVec->push_back(*((MarketDataDO*)rawParams[0]));

	return ret;
}