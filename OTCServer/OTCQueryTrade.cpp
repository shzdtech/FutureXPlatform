/***********************************************************************
 * Module:  OTCQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年10月15日 16:08:36
 * Purpose: Implementation of the class OTCQueryOrder
 ***********************************************************************/

#include "OTCQueryTrade.h"

#include "../message/MessageUtility.h"
#include "OTCWorkerProcessor.h"

#include "../dataobject/FieldName.h"
#include "../dataobject/TemplateDO.h"
#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../message/TemplateMessageProcessor.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"

#include "../dataobject/TradeRecordDO.h"
#include "../databaseop/TradeDAO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCQueryOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of OTCQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryTrade::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);
	auto& exchangeid = stdo->TryFind(STR_EXCHANGE_ID, EMPTY_STRING);

	char today[20];
	auto time = session->getUserInfo()->getLoginTime();
	std::strftime(today, 20, "%Y-%m-%d", std::localtime(&time));
	auto& tmstart = stdo->TryFind(STR_TIME_START, today);
	auto& tmend = stdo->TryFind(STR_TIME_END, today);

	auto tradeVec_Ptr = TradeDAO::QueryTrade(session->getUserInfo()->getUserId(),
		ContractKey(exchangeid, instrumentid), tmstart, tmend);
	ThrowNotFoundExceptionIfEmpty(tradeVec_Ptr);

	if (auto wkProcPtr = std::static_pointer_cast<TemplateMessageProcessor>(session->getProcessor()))
	{
		auto lastit = std::prev(tradeVec_Ptr->end());
		for (auto it = tradeVec_Ptr->begin(); it != tradeVec_Ptr->end(); it++)
		{
			auto trade_ptr = std::make_shared<TradeRecordDO>(*it);
			trade_ptr->HasMore = it != lastit;

			wkProcPtr->SendDataObject(session, MSG_ID_QUERY_TRADE, reqDO->SerialId, trade_ptr);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCQueryTrade::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of OTCQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryTrade::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);

	return std::make_shared<OrderDO>(orderDO);
}