/***********************************************************************
 * Module:  OTCQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年10月15日 16:08:36
 * Purpose: Implementation of the class OTCQueryOrder
 ***********************************************************************/

#include "OTCQueryOrder.h"

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

#include "../dataobject/OrderDO.h"
#include "../databaseop/OTCOrderDAO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCQueryOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of OTCQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);

	auto ordervec_ptr = OTCOrderDAO::QueryTodayOrder(session->getUserInfo()->getUserId(),
		ContractKey(EMPTY_STRING, instrumentid));
	ThrowNotFoundExceptionIfEmpty(ordervec_ptr);

	if (auto wkProcPtr = std::static_pointer_cast<TemplateMessageProcessor>(session->getProcessor()))
	{
		auto lastit = std::prev(ordervec_ptr->end());
		for (auto it = ordervec_ptr->begin(); it != ordervec_ptr->end(); it++)
		{
			auto order_ptr = std::make_shared<OrderDO>(*it);
			order_ptr->HasMore = it != lastit;

			wkProcPtr->SendDataObject(session, MSG_ID_QUERY_ORDER, reqDO->SerialId, order_ptr);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCQueryOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of OTCQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);

	return std::make_shared<OrderDO>(orderDO);
}