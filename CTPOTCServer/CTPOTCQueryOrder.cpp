/***********************************************************************
 * Module:  CTPOTCQueryOrder.cpp
 * Author:  milk
 * Modified: 2015年10月15日 16:08:36
 * Purpose: Implementation of the class CTPOTCQueryOrder
 ***********************************************************************/

#include "CTPOTCQueryOrder.h"
#include "../CTPServer/CTPUtility.h"
#include "../CTPServer/CTPAppContext.h"
#include "CTPWorkerProcessorID.h"
#include "CTPOTCWorkerProcessor.h"

#include "../dataobject/FieldName.h"
#include "../dataobject/TemplateDO.h"
#include "../message/BizError.h"
#include "../message/message_marco.h"
#include "../message/DefMessageID.h"
#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>

#include "../dataobject/OrderDO.h"
#include "../databaseop/OTCOrderDAO.h"


////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryOrder::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryOrder::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryOrder::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckLogin(session);

	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;

	auto& instrumentid = TUtil::FirstNamedEntry(STR_INSTRUMENT_ID, data, EMPTY_STRING);
	
	if (auto ordervec_ptr = OTCOrderDAO::QueryTodayOrder(session->getUserInfo()->getUserId(),
		ContractKey(EMPTY_STRING, instrumentid)))
	{
		if (auto proc = std::static_pointer_cast<CTPOTCWorkerProcessor>
			(CTPAppContext::FindServerProcessor(WORKPROCESSOR_OTC)))
		{
			for (auto& it : *ordervec_ptr)
			{
				if (it.OrderStatus == OrderStatus::OPENNING)
					proc->RegisterOTCOrderListener(it.OrderID, (IMessageSession*)session);
				OnResponseProcMarco(session->getProcessor(), MSG_ID_QUERY_ORDER, &it);
			}
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCQueryOrder::HandleResponse(ParamVector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCQueryOrder::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCQueryOrder::HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);

	return std::make_shared<OrderDO>(orderDO);
}