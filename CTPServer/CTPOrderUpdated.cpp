#include "CTPOrderUpdated.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"
#include "../message/MessageUtility.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../dataobject/OrderDO.h"
#include "tradeapi/ThostFtdcTraderApi.h"

dataobj_ptr CTPOrderUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	if (auto pOrder = (CThostFtdcOrderField*)rawRespParams[0])
	{

		OrderDO_Ptr orderPtr;
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
		{
			orderPtr = pWorkerProc->GetUserOrderContext().FindOrder(CTPUtility::ToUInt64(pOrder->OrderSysID));
			orderPtr = CTPUtility::ParseRawOrder(pOrder, orderPtr);
			if (orderPtr && orderPtr->OrderSysID)
			{
				pWorkerProc->GetUserOrderContext().UpsertOrder(orderPtr->OrderSysID, orderPtr);
			}
		}
		else
		{
			orderPtr = CTPUtility::ParseRawOrder(pOrder);
		}

		if (orderPtr)
		{
			auto msgId = CTPUtility::ParseMessageID(orderPtr->OrderStatus);
			auto pProcessor = (TemplateMessageProcessor*)session->getProcessor().get();
			pProcessor->SendDataObject(session, msgId, serialId, orderPtr);
		}
	}

	return nullptr;
}
