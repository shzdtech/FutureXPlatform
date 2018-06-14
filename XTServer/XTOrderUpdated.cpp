#include "XTOrderUpdated.h"
#include "XTUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"
#include "../message/MessageUtility.h"
#include "XTTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../dataobject/OrderDO.h"
#include "../message/DefMessageID.h"
#include "CTPAPISwitch.h"

dataobj_ptr XTOrderUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	OrderDO_Ptr orderPtr;
	if (auto pOrder = (CThostFtdcOrderField*)rawRespParams[0])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			if (orderPtr = pWorkerProc->GetUserOrderContext().FindOrder(XTUtility::ToUInt64(pOrder->OrderSysID)))
			{
				orderPtr = XTUtility::ParseRawOrder(pOrder, orderPtr);
			}
			else
			{
				orderPtr = pWorkerProc->RefineOrder(pOrder);
				if (orderPtr && orderPtr->OrderSysID)
				{
					pWorkerProc->GetUserOrderContext().UpsertOrder(orderPtr->OrderSysID, orderPtr);
				}
			}
		}
		else
		{
			orderPtr = XTUtility::ParseRawOrder(pOrder);
		}

		if (session->getUserInfo().getUserId() != pOrder->UserID)
		{
			orderPtr.reset();
		}

		if (orderPtr)
		{
			if (auto msgId = XTUtility::ParseOrderMessageID(orderPtr->OrderStatus))
			{
				auto pProcessor = (TemplateMessageProcessor*)msgProcessor.get();
				auto sid = msgId == MSG_ID_ORDER_CANCEL ? orderPtr->OrderSysID : serialId;
				pProcessor->SendDataObject(session, msgId, sid, orderPtr);
			}

			if (!orderPtr->OrderSysID)
				orderPtr.reset();
		}
	}

	return orderPtr;
}
