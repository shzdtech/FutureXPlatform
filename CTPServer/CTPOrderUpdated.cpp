#include "CTPOrderUpdated.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"
#include "../message/MessageUtility.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../dataobject/OrderDO.h"
#include "../message/DefMessageID.h"
#include "CTPAPISwitch.h"

dataobj_ptr CTPOrderUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	OrderDO_Ptr orderPtr;
	if (auto pOrder = (CThostFtdcOrderField*)rawRespParams[0])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			if (orderPtr = pWorkerProc->GetUserOrderContext().FindOrder(CTPUtility::ToUInt64(pOrder->OrderSysID)))
			{
				orderPtr = CTPUtility::ParseRawOrder(pOrder, orderPtr);
				if (orderPtr->IsSystemUserId())
				{
					orderPtr->SetUserID(session->getUserInfo().getUserId());
				}
			}
			else
			{
				orderPtr = pWorkerProc->RefineOrder(pOrder);
				if (orderPtr && orderPtr->OrderSysID)
				{
					if (orderPtr->IsSystemUserId())
					{
						orderPtr->SetUserID(session->getUserInfo().getUserId());
					}
					pWorkerProc->GetUserOrderContext().UpsertOrder(orderPtr->OrderSysID, orderPtr);
				}
			}
		}
		else
		{
			orderPtr = CTPUtility::ParseRawOrder(pOrder);
			if (orderPtr->IsSystemUserId())
			{
				orderPtr->SetUserID(session->getUserInfo().getUserId());
			}
		}

		if (orderPtr)
		{
			if (auto msgId = CTPUtility::ParseOrderMessageID(orderPtr->OrderStatus))
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
