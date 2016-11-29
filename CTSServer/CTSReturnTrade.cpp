/***********************************************************************
* Module:  CTSReturnTrade.cpp
* Author:  milk
* Modified: 2015Äê11ÔÂ8ÈÕ 22:26:40
* Purpose: Implementation of the class CTSReturnTrade
***********************************************************************/

#include "CTSReturnTrade.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TemplateDO.h"
#include "../message/BizError.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSReturnTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSReturnTrade::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSReturnTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSReturnTrade::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSReturnTrade::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSReturnTrade::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	auto pTradeDO = (TradeRecordDO*)rawParams[0];
	return std::make_shared<TradeRecordDO>(*pTradeDO);
}