/***********************************************************************
 * Module:  CTPOTCNewOrder.cpp
 * Author:  milk
 * Modified: 2015年10月14日 0:08:44
 * Purpose: Implementation of the class CTPOTCNewOrder
 ***********************************************************************/

#include "CTPOTCNewOrder.h"
#include <memory>

#include "../message/GlobalProcessorRegistry.h"
#include "../CTPServer/CTPWorkerProcessorID.h"
#include "CTPOTCWorkerProcessor.h"
#include "../dataobject/OrderDO.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCNewOrder::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
 // Purpose:    Implementation of CTPOTCNewOrder::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCNewOrder::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	CheckLogin(session);

	auto& orderDO = *((OrderDO*)reqDO.get());
	orderDO.SetUserID(session->getUserInfo()->getUserId());
	if (auto wkProcPtr = std::static_pointer_cast<CTPOTCWorkerProcessor>
		(GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::WORKPROCESSOR_OTC)))
	{
		int rtn = wkProcPtr->OTCNewOrder(orderDO);
		if (rtn != 0)
			throw BizError(rtn, "Creating OTC Order failed", orderDO.SerialId);
	}

	return reqDO;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPOTCNewOrder::HandleResponse(const uint32_t serialId, param_vector rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPOTCNewOrder::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCNewOrder::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	auto& orderDO = *((OrderDO*)rawRespParams[0]);
	orderDO.SerialId = serialId;

	return std::make_shared<OrderDO>(orderDO);
}