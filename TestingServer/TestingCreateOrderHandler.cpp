#include "TestingCreateOrderHandler.h"
#include "TestingServerMessageProcessor.h"
#include "TestingWorkProcessor.h"
#include "../message/MessageUtility.h"

#include "../systemsettings/AppContext.h"
#include "../message/DefMessageID.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/TradeRecordDO.h"

#include "../ordermanager/OrderSeqGen.h"


dataobj_ptr TestingCreateOrderHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	_tradingTask = std::async(std::launch::async, [reqDO, serialId, msgProcessor]()
	{
		if (auto sessionptr = msgProcessor->getMessageSession())
		{
			auto pProc = MessageUtility::WorkerProcessorPtr<TestingWorkProcessor>(msgProcessor);
			auto pDO = (OrderRequestDO*)reqDO.get();
			auto orderptr = std::make_shared<OrderDO>(*pDO);
			orderptr->SetUserID(sessionptr->getUserInfo().getUserId());
			orderptr->OrderID = orderptr->OrderSysID = OrderSeqGen::GenOrderID();
			pProc->GetUserOrderContext().UpsertOrder(orderptr->OrderSysID, *orderptr);
			pProc->SendDataObject(sessionptr, MSG_ID_ORDER_NEW, serialId, orderptr);


			auto tradeptr = std::make_shared<TradeRecordDO>
				(orderptr->ExchangeID(), orderptr->InstrumentID(),
					sessionptr->getUserInfo().getName(), "TestPortfolio");
			tradeptr->Direction = orderptr->Direction;
			tradeptr->OpenClose = orderptr->OpenClose;
			

			while (orderptr->VolumeRemain() > 0)
			{
				std::this_thread::sleep_for(std::chrono::seconds(3));

				orderptr->OrderStatus = OrderStatusType::PARTIAL_TRADED;
				orderptr->VolumeTraded++;

				if (orderptr->VolumeRemain() < 1)
					orderptr->OrderStatus = OrderStatusType::ALL_TRADED;


				pProc->SendDataObject(sessionptr, MSG_ID_ORDER_UPDATE, serialId, orderptr);
				tradeptr->SetUserID(sessionptr->getUserInfo().getUserId());
				tradeptr->Volume = 1;
				tradeptr->TradeID = AppContext::GenNextSeq();
				tradeptr->OrderID = orderptr->OrderID;
				tradeptr->OrderSysID = orderptr->OrderSysID;

				pProc->GetUserTradeContext().InsertTrade(tradeptr);

				pProc->SendDataObject(sessionptr, MSG_ID_TRADE_RTN, serialId, tradeptr);
			}
		}
	});

	return nullptr;
}
