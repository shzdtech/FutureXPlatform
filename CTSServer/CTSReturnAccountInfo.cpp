/***********************************************************************
* Module:  CTSReturnAccountInfo.cpp
* Author:  xiaoyu
* Modified: 2016Äê11ÔÂ8ÈÕ 22:26:40
* Purpose: Implementation of the class CTSReturnTrade
***********************************************************************/

#include "CTSReturnAccountInfo.h"
#include "../dataobject/AccountInfoDO.h"
#include "../dataobject/TemplateDO.h"
#include "../message/BizError.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSReturnTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTSReturnAccountInfo::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSReturnAccountInfo::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSReturnAccountInfo::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTSReturnAccountInfo::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSReturnAccountInfo::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto pAccountInfoDO = (AccountInfoDO*)rawParams[0];
	return std::make_shared<AccountInfoDO>(*pAccountInfoDO);
}