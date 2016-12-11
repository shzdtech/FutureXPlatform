/***********************************************************************
* Module:  CTSQueryOrder.cpp
* Author:  xiaoyu
* Modified: 2016Äê11ÔÂ25ÈÕ 12:57:18
* Purpose: Implementation of the class CTSQueryPosition
***********************************************************************/

#include "CTSQueryPosition.h"
#include "CTSAPIWrapper.h"
#include "../dataobject/UserPositionDO.h"






////////////////////////////////////////////////////////////////////////
// Name:       CTSQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTSQueryPosition::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSQueryPosition::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto api = (CTSAPIWrapper*)rawAPI;
	auto vectorPtr = api->Impl()->QueryPosition();

	ThrowNotFoundExceptionIfEmpty(vectorPtr);

	auto lastidx = vectorPtr->size() - 1;
	for (int i = 0; i <= lastidx; i++)
	{
		auto rspPositionPtr = std::make_shared<UserPositionExDO>(vectorPtr->at(i));
		rspPositionPtr->HasMore = i < lastidx;
		OnResponseProcMacro(msgProcessor, MSG_ID_QUERY_POSITION, serialId, &rspPositionPtr);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSQueryPosition::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTSQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSQueryPosition::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	UserPositionExDO_Ptr position_ptr = *((UserPositionExDO_Ptr*)rawParams[0]);

	return position_ptr;
}