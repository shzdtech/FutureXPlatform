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
#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"

#include "../dataobject/OrderDO.h"
#include "../databaseop/OTCOrderDAO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       OTCQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
 // Purpose:    Implementation of OTCQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryOrder::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	CheckLogin(session);

	auto stdo = (StringMapDO<std::string>*)reqDO.get();

	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);

	auto ordervec_ptr = OTCOrderDAO::QueryTodayOrder(session->getUserInfo().getUserId(),
		ContractKey(EMPTY_STRING, instrumentid));
	ThrowNotFoundExceptionIfEmpty(ordervec_ptr);

	if (auto pWorkerProc = (TemplateMessageProcessor*)msgProcessor.get())
	{
		auto lastidx = ordervec_ptr->size() - 1;
		for (int i = 0; i <= lastidx; i++)
		{
			auto rspOrderPtr = std::make_shared<OrderDO>(ordervec_ptr->at(i));
			rspOrderPtr->HasMore = i < lastidx;
			pWorkerProc->SendDataObject(session, MSG_ID_QUERY_ORDER, serialId, rspOrderPtr);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCQueryOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
// Purpose:    Implementation of OTCQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr OTCQueryOrder::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}