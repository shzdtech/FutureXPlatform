/***********************************************************************
 * Module:  CTPOTCQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年10月15日 16:08:36
 * Purpose: Implementation of the class CTPOTCQueryOrder
 ***********************************************************************/

#include "CTPOTCQueryOrder.h"

#include "../message/MessageUtility.h"
#include "../CTPServer/CTPWorkerProcessorID.h"
#include "CTPOTCWorkerProcessor.h"

#include "../dataobject/FieldName.h"
#include "../dataobject/TemplateDO.h"
#include "../message/BizError.h"
#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"

#include "../dataobject/OrderDO.h"
#include "../databaseop/OTCOrderDAO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCQueryOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPOTCQueryOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryOrder::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto stdo = (MapDO<std::string>*)reqDO.get();

	auto& instrumentid = stdo->TryFind(STR_INSTRUMENT_ID, EMPTY_STRING);

	auto ordervec_ptr = OTCOrderDAO::QueryTodayOrder(session->getUserInfo()->getUserId(),
		ContractKey(EMPTY_STRING, instrumentid));

	ThrowNotFoundException(ordervec_ptr);
	if (auto wkProcPtr =
		MessageUtility::FindGlobalProcessor<CTPOTCWorkerProcessor>(CTPWorkerProcessorID::WORKPROCESSOR_OTC))
	{
		ThrowNotFoundException(ordervec_ptr);

		auto lastit = std::prev(ordervec_ptr->end());
		for (auto it = ordervec_ptr->begin(); it != ordervec_ptr->end(); it++)
		{
			auto order_ptr = std::make_shared<OrderDO>(*it);
			order_ptr->SerialId = reqDO->SerialId;
			order_ptr->HasMore = it != lastit;

			wkProcPtr->SendDataObject(session, MSG_ID_QUERY_ORDER, order_ptr);
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryOrder::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);
	orderDO.SerialId = serialId;

	return std::make_shared<OrderDO>(orderDO);
}