#include "TestingPositionHandler.h"
#include "../dataobject/UserPositionDO.h"
#include "../bizutility/ContractCache.h"
#include "../message/message_macro.h"
#include "../message/DefMessageID.h"

dataobj_ptr TestingPositionHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	static VectorDO_Ptr<InstrumentDO> contracts;

	if (!contracts)
		contracts = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).AllInstruments();

	int lastidx = std::min((int)contracts->size(), 15);
	for (int i = 0; i <= lastidx; i++)
	{
		UserPositionExDO position(contracts->at(i).ExchangeID(), contracts->at(i).InstrumentID());
		position.HasMore = i < lastidx;
		position.YdPosition = std::rand() % 10;
		position.TdPosition = std::rand() % 10;
		position.YdProfit = std::rand();
		position.TdProfit = std::rand();
		position.PositionDateFlag = PositionDateFlagType::PSD_TODAY;
		position.YdCost = std::rand();
		position.TdCost = std::rand();
		position.CloseAmount = std::rand();
		position.CloseProfit = std::rand();
		position.OpenVolume = std::rand();
		position.OpenAmount = std::rand();
		position.OpenCost = std::rand();
		position.Direction = std::rand() % 2 ? PositionDirectionType::PD_LONG : PositionDirectionType::PD_SHORT;

		OnResponseProcMacro(session->getProcessor(), MSG_ID_QUERY_POSITION, serialId, &position);
	}

	return nullptr;
}

dataobj_ptr TestingPositionHandler::HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session)
{
	auto pDO = std::make_shared<UserPositionExDO>(*(UserPositionExDO*)rawParams[0]);

	std::thread t([pDO, serialId, session]()
	{
		if (auto sessionptr = session->getProcessor()->LockMessageSession())
		{
			for (int i = 0; i < 15; i++)
			{
				pDO->TdPosition++;
				std::this_thread::sleep_for(std::chrono::seconds(2));
				((TemplateMessageProcessor*)sessionptr->getProcessor().get())
					->SendDataObject(sessionptr.get(), MSG_ID_POSITION_UPDATED, serialId, pDO);
			}
		}
	});
	t.detach();

	return pDO;
}