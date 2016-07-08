/***********************************************************************
 * Module:  CTSServiceFactory.cpp
 * Author:  milk
 * Modified: 2015年11月8日 15:47:20
 * Purpose: Implementation of the class CTSServiceFactory
 ***********************************************************************/

#include "CTSServiceFactory.h"
#include "CTSProcessor.h"
#include "cts_handlers.h"
#include "../message/DefMessageID.h"
#include "../dataobject/AbstractDataSerializerFactory.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of CTSServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTSServiceFactory::CreateMessageHandlers(void)
{
	std::map<uint, IMessageHandler_Ptr> msg_hdl_map;

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTSLoginHandler>();

	///Market Data
	msg_hdl_map[MSG_ID_SUB_MARKETDATA] = std::make_shared<CTSSubscribeMarketData>();

	msg_hdl_map[MSG_ID_RET_MARKETDATA] = std::make_shared<CTSReturnMarketData>();

	///Trader
	msg_hdl_map[MSG_ID_ORDER_NEW] = std::make_shared<CTSNewOrder>();

	msg_hdl_map[MSG_ID_ORDER_CANCEL] = std::make_shared<CTSCancelOrder>();

	msg_hdl_map[MSG_ID_ORDER_UPDATE] = std::make_shared<CTSQueryOrder>();

	return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTSServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTSServiceFactory::CreateDataSerializers(void)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSServiceFactory::CreateWorkProcessor()
// Purpose:    Implementation of CTSServiceFactory::CreateWorkProcessor()
// Return:     std::map<uint, IProcessorBase_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<std::string, IProcessorBase_Ptr> CTSServiceFactory::CreateWorkProcessor(void)
{
	std::map<std::string, IProcessorBase_Ptr> workproc_map;
	return workproc_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTSServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTSServiceFactory::CreateMessageProcessor(void)
{
	return std::make_shared<CTSProcessor>(_configMap);
}