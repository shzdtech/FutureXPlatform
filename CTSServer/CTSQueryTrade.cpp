/***********************************************************************
* Module:  CTSQueryTrade.cpp
* Author:  milk
* Modified: 2015��12��10�� 12:57:18
* Purpose: Implementation of the class CTSQueryOrder
***********************************************************************/

#include "CTSQueryTrade.h"
#include "CTSAPIWrapper.h"




////////////////////////////////////////////////////////////////////////
// Name:       CTSQueryTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTSQueryTrade::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSQueryTrade::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto api = (CTSAPIWrapper*)rawAPI;
	auto vectorPtr = api->Impl()->QueryOTCUserTrades();

	ThrowNotFoundExceptionIfEmpty(vectorPtr);

	auto lastidx = vectorPtr->size() - 1;
	for (int i = 0; i <= lastidx; i++)
	{
		auto rspTradePtr = std::make_shared<TradeRecordDO>(vectorPtr->at(i));
		rspTradePtr->HasMore = i < lastidx;
		OnResponseProcMacro(msgProcessor, MSG_ID_QUERY_TRADE, serialId, &rspTradePtr);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, IMessageProcessor* msgProcessor
// Purpose:    Implementation of CTSQueryTrade::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTSQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	TradeRecordDO_Ptr trade_ptr = *((TradeRecordDO_Ptr*)rawParams[0]);

	return trade_ptr;
}