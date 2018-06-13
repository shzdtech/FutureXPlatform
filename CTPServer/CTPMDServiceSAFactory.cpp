/***********************************************************************
 * Module:  CTPMDServiceSAFactory.cpp
 * Author:  milk
 * Modified: 2014年10月7日 0:02:09
 * Purpose: Implementation of the class CTPMDServiceSAFactory
 ***********************************************************************/

#include "CTPMDServiceSAFactory.h"
#include "CTPMarketDataSAProcessor.h"
#include "ctp_bizhandlers.h"

#include "CTPTradeWorkerProcessor.h"
#include "../systemsettings/AppContext.h"
#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../common/Attribute_Key.h"
#include "../dataserializer/AbstractDataSerializerFactory.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDServiceSAFactory::CreateMessageHandlers()
// Purpose:    Implementation of CTPMDServiceSAFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPMDServiceSAFactory::CreateMessageHandlers(IServerContext* serverCtx)
{
    std::map<uint, IMessageHandler_Ptr> msg_hdl_map;
 
	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMessageHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPMDLoginSAHandler>();

	msg_hdl_map[MSG_ID_SUB_MARKETDATA] = std::make_shared<CTPSubMarketDataSA>();

	msg_hdl_map[MSG_ID_UNSUB_MARKETDATA] = std::make_shared<CTPUnsubMarketDataSA>();

	msg_hdl_map[MSG_ID_QUERY_INSTRUMENT] = std::make_shared<CTPQueryInstrument>();

    return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDServiceSAFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPMDServiceSAFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPMDServiceSAFactory::CreateDataSerializers(IServerContext* serverCtx)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDServiceSAFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPMDServiceSAFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPMDServiceSAFactory::CreateMessageProcessor(IServerContext* serverCtx)
{
	auto pWorker = std::static_pointer_cast<CTPMarketDataSAProcessor>(serverCtx->getWorkerProcessor());
	auto ret = std::make_shared<CTPProcessor>(pWorker->RawAPI_Ptr());
	ret->Initialize(serverCtx);
	return ret;
}

IMessageProcessor_Ptr CTPMDServiceSAFactory::CreateWorkerProcessor(IServerContext* serverCtx)
{
	if (!serverCtx->getWorkerProcessor())
	{
		auto mktDataMap = std::static_pointer_cast<MarketDataDOMap>(AppContext::GetData(STR_KEY_APP_MARKETDATA));
		auto worker_ptr = std::make_shared<CTPMarketDataSAProcessor>(serverCtx, mktDataMap.get());
		worker_ptr->Initialize(serverCtx);
		serverCtx->setWorkerProcessor(worker_ptr);
		serverCtx->setSubTypeWorkerPtr(static_cast<CTPMarketDataSAProcessor*>(worker_ptr.get()));
	}

	return serverCtx->getWorkerProcessor();
}

void CTPMDServiceSAFactory::SetServerContext(IServerContext * serverCtx)
{
	MessageServiceFactory::SetServerContext(serverCtx);
	auto mktDataMap = AppContext::GetData(STR_KEY_APP_MARKETDATA);
	if (!mktDataMap)
	{
		mktDataMap = std::make_shared<MarketDataDOMap>();
		AppContext::SetData(STR_KEY_APP_MARKETDATA, mktDataMap);
	}
}