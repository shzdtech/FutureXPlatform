#include "TestingCreateOrderHandler.h"
#include "TestingServerMessageProcessor.h"

#include "../systemsettings/AppContext.h"
#include "../message/DefMessageID.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/TradeRecordDO.h"


dataobj_ptr TestingCreateOrderHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI * rawAPI, ISession * session)
{
	auto pDO = (OrderRequestDO*)reqDO.get();

	_tradingTask = std::async(std::launch::async, [pDO, session]()
	{
		if (auto sessionptr = session->getProcessor()->LockMessageSession())
		{

			auto orderptr = std::make_shared<OrderDO>(*pDO);
			auto tradeptr = std::make_shared<TradeRecordDO>
				(orderptr->ExchangeID(), orderptr->InstrumentID(),
					session->getUserInfo()->getName(), "TestPortfolio");
			tradeptr->Direction = orderptr->Direction;
			tradeptr->OpenClose = orderptr->OpenClose;

			orderptr->VolumeRemain = orderptr->Volume;
			auto pProc = (TestingServerMessageProcessor*)sessionptr->getProcessor().get();

			while (orderptr->VolumeRemain > 0)
			{
				std::this_thread::sleep_for(std::chrono::seconds(3));

				orderptr->OrderStatus = OrderStatus::PARTIAL_TRADED;
				orderptr->VolumeRemain--;
				orderptr->VolumeTraded = orderptr->Volume - orderptr->VolumeRemain;

				if (orderptr->VolumeRemain < 1)
					orderptr->OrderStatus = OrderStatus::ALL_TRADED;


				pProc->SendDataObject(sessionptr.get(), MSG_ID_ORDER_UPDATE, pDO->SerialId, orderptr);

				tradeptr->Volume = 1;
				tradeptr->TradeID = AppContext::GenNextSeq();
				pProc->SendDataObject(sessionptr.get(), MSG_ID_TRADE_RTN, pDO->SerialId, tradeptr);
			}
		}
	});

	return std::make_shared<OrderDO>(*pDO);
}
