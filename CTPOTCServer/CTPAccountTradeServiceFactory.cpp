/***********************************************************************
 * Module:  CTPAccountTradeServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年8月31日 19:44:34
 * Purpose: Implementation of the class CTPAccountTradeServiceFactory
 ***********************************************************************/

#include "CTPAccountTradeServiceFactory.h"
#include "../CTPServer/CTPWorkerProcessorID.h"
#include "ctpotc_bizhandlers.h"
#include "../CTPServer/ctp_bizhandlers.h"
#include "../CTPServer/CTPTradeWorkerProcessor.h"
#include "../message/MessageUtility.h"
#include "../common/Attribute_Key.h"

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../dataobject/AbstractDataSerializerFactory.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPAccountTradeServiceFactory::CreateMessageHandlers()
 // Purpose:    Implementation of CTPAccountTradeServiceFactory::CreateMessageHandlers()
 // Return:     std::map<uint, IMessageHandler_Ptr>
 ////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPAccountTradeServiceFactory::CreateMessageHandlers(void)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map =
		CTPTradeServiceFactory::CreateMessageHandlers();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPAccountLogin>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPAccountTradeServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPAccountTradeServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPAccountTradeServiceFactory::CreateDataSerializers(void)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPAccountTradeServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPAccountTradeServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPAccountTradeServiceFactory::CreateMessageProcessor(void)
{
	auto pWorker = MessageUtility::FindGlobalProcessor<CTPTradeWorkerProcessor>(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT);
	return std::make_shared<CTPProcessor>(pWorker->RawAPI_Ptr());
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPAccountTradeServiceFactory::CreateWorkProcessor()
// Purpose:    Implementation of CTPAccountTradeServiceFactory::CreateWorkProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<std::string, IProcessorBase_Ptr> CTPAccountTradeServiceFactory::CreateWorkProcessor(void)
{
	std::map<std::string, IProcessorBase_Ptr> workerProcMap;
	IProcessorBase_Ptr prcPtr;

	if (!(prcPtr = GlobalProcessorRegistry::FindProcessor(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT)))
	{
		auto worker_ptr = std::make_shared<CTPTradeWorkerProcessor>(_configMap);
		worker_ptr->Initialize();
		GlobalProcessorRegistry::RegisterProcessor(CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT, worker_ptr);
		prcPtr = worker_ptr;
	}
	workerProcMap[CTPWorkerProcessorID::TRADE_SHARED_ACCOUNT] = prcPtr;

	return workerProcMap;
}

bool CTPAccountTradeServiceFactory::Load(const std::string& configFile, const std::string& param)
{
	return CTPTradeServiceFactory::Load(configFile, param);
}


void CTPAccountTradeServiceFactory::SetServerContext(IContextAttribute * serverCtx)
{
	CTPTradeServiceFactory::SetServerContext(serverCtx);
	//serverCtx->setAttribute(STR_KEY_SERVER_PRICING_DATACONTEXT, std::make_shared<PricingDataContext>());
}
