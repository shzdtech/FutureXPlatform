/***********************************************************************
 * Module:  CTPMDServiceFactory.cpp
 * Author:  milk
 * Modified: 2014年10月7日 0:02:09
 * Purpose: Implementation of the class CTPMDServiceFactory
 ***********************************************************************/

#include "CTPMDServiceFactory.h"
#include "CTPMarketDataProcessor.h"
#include "ctp_bizhandlers.h"
#include "CTPConstant.h"

#include "../message/EchoMessageHandler.h"
#include "../message/EchoMessageSerializer.h"
#include "../message/DefMessageID.h"
#include "../message/SysParam.h"

#include "../dataserializer/AbstractDataSerializerFactory.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDServiceFactory::CreateMessageHandlers()
// Purpose:    Implementation of CTPMDServiceFactory::CreateMessageHandlers()
// Return:     std::map<uint, IMessageHandler_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IMessageHandler_Ptr> CTPMDServiceFactory::CreateMessageHandlers(void)
{
    std::map<uint, IMessageHandler_Ptr> msg_hdl_map;
 
	msg_hdl_map[MSG_ID_ECHO] = std::make_shared<EchoMessageHandler>();

	msg_hdl_map[MSG_ID_LOGIN] = std::make_shared<CTPMDLoginHandler>();

	msg_hdl_map[MSG_ID_SUB_MARKETDATA] = std::make_shared<CTPSubMarketData>();

	msg_hdl_map[MSG_ID_UNSUB_MARKETDATA] = std::make_shared<CTPUnsubMarketData>();

	msg_hdl_map[MSG_ID_RET_MARKETDATA] = std::make_shared<CTPDepthMarketData>();

    return msg_hdl_map;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDServiceFactory::CreateDataSerializers()
// Purpose:    Implementation of CTPMDServiceFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<uint, IDataSerializer_Ptr> CTPMDServiceFactory::CreateDataSerializers(void)
{
	std::map<uint, IDataSerializer_Ptr> ret;
	AbstractDataSerializerFactory::Instance()->CreateDataSerializers(ret);
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDServiceFactory::CreateMessageProcessor()
// Purpose:    Implementation of CTPMDServiceFactory::CreateMessageProcessor()
// Return:     IMessageProcessor_Ptr
////////////////////////////////////////////////////////////////////////

IMessageProcessor_Ptr CTPMDServiceFactory::CreateMessageProcessor(void)
{
	auto ret = std::make_shared<CTPMarketDataProcessor>(_configMap);
	ret->Initialize();
	return ret;
}

bool CTPMDServiceFactory::Load(const std::string& configFile, const std::string& param)
{
	bool ret = MessageServiceFactory::Load(configFile, param);
	std::string frontserver;
	if (SysParam::Contains(CTP_MD_SERVER))
	{
		_configMap[STR_FRONT_SERVER] = SysParam::Get(CTP_MD_SERVER);
	}

	return ret;
}