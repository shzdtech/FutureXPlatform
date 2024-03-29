/***********************************************************************
 * Module:  CTSReturnMarketData.cpp
 * Author:  milk
 * Modified: 2015年11月8日 22:26:40
 * Purpose: Implementation of the class CTSReturnMarketData
 ***********************************************************************/

#include "CTSReturnMarketData.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/TemplateDO.h"
#include "../message/BizError.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSReturnMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTSReturnMarketData::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSReturnMarketData::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSReturnMarketData::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of CTSReturnMarketData::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSReturnMarketData::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto pMdDO = (MarketDataDO*)rawParams[0];
	return std::make_shared<MarketDataDO>(*pMdDO);
}