/***********************************************************************
* Module:  CTSReturnPosition.cpp
* Author:  milk
* Modified: 2015��11��8�� 22:26:40
* Purpose: Implementation of the class CTSReturnTrade
***********************************************************************/

#include "CTSReturnPosition.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TemplateDO.h"
#include "../message/BizError.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSReturnPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSReturnPosition::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSReturnPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSReturnPosition::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTSReturnPosition::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSReturnPosition::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	auto pPositionDO = (UserPositionExDO*)rawParams[0];
	return std::make_shared<UserPositionExDO>(*pPositionDO);
}