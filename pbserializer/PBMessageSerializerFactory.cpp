/***********************************************************************
 * Module:  PBMessageSerializerFactory.cpp
 * Author:  milk
 * Modified: 2015年9月18日 10:26:49
 * Purpose: Implementation of the class PBMessageSerializerFactory
 ***********************************************************************/

#include "PBMessageSerializerFactory.h"
#include "pbserializers.h"
#include "../message/DefMessageID.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBMessageSerializerFactory::PBMessageSerializerFactory()
 // Purpose:    Implementation of PBMessageSerializerFactory::PBMessageSerializerFactory()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

PBMessageSerializerFactory::PBMessageSerializerFactory()
{
	//Common
	_serializer_map[MSG_ID_ERROR] = PBBizErrorSerializer::Instance();
	_serializer_map[MSG_ID_SESSION_CREATED] = PBEchoMsgSerializer::Instance();
	_serializer_map[MSG_ID_ECHO] = PBEchoMsgSerializer::Instance();
	_serializer_map[MSG_ID_LOGIN] = std::make_shared<PBCombineSerializer>
		(PBUserInfoSerializer::Instance(), PBStringTableSerializer::Instance());

	//Account
	_serializer_map[MSG_ID_USER_INFO] = std::make_shared<PBCombineSerializer>
		(PBUserInfoSerializer::Instance(), PBStringTableSerializer::Instance());
	_serializer_map[MSG_ID_USER_NEW] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBUserInfoSerializer::Instance());
	_serializer_map[MSG_ID_USER_INFO_UPDATE] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBUserInfoSerializer::Instance());

	//Market Data
	_serializer_map[MSG_ID_SUB_MARKETDATA] = PBSubMarketDataSerializer::Instance();
	_serializer_map[MSG_ID_UNSUB_MARKETDATA] = PBSubMarketDataSerializer::Instance();
	_serializer_map[MSG_ID_RET_MARKETDATA] = std::make_shared<PBMarketDataSerializer>();

	//Trade
	_serializer_map[MSG_ID_ORDER_NEW] = PBOrderSerializer::Instance();
	_serializer_map[MSG_ID_ORDER_CANCEL] = PBOrderSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_ACCOUNT_INFO] = PBAccountInfoSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_EXCHANGE] = PBExchangeSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_INSTRUMENT] = PBInstrumentSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_ORDER] = std::make_shared<PBCombineSerializer>
		(PBOrderSerializer::Instance(), PBStringTableSerializer::Instance());
	_serializer_map[MSG_ID_ORDER_UPDATE] = PBOrderSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_POSITION] = PBUserPositionSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_TRADE] = PBTradeRecordSerializer::Instance();
	_serializer_map[MSG_ID_TRADE_RTN] = PBTradeRecordSerializer::Instance();
	_serializer_map[MSG_ID_SETTLEMENT_INFO_CONFIRM] = PBStringTableSerializer::Instance();

	//OTC
	_serializer_map[MSG_ID_SUB_PRICING] = std::make_shared<PBCombineSerializer>
		(PBPricingDataSerializer::Instance(), PBStringTableSerializer::Instance());
	_serializer_map[MSG_ID_RTN_PRICING] = PBPricingDataSerializer::Instance();
	_serializer_map[MSG_ID_MODIFY_USER_PARAM] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBUserParamSerializer::Instance());
	_serializer_map[MSG_ID_QUERY_TRADINGDESK] = std::make_shared<PBCombineSerializer>
		(PBUserInfoListSerializer::Instance(), PBStringTableSerializer::Instance());
	_serializer_map[MSG_ID_QUERY_STRATEGY] = std::make_shared<PBCombineSerializer>
		(PBStrategySerializer::Instance(), PBStringTableSerializer::Instance());
	_serializer_map[MSG_ID_QUERY_CONTRACT_PARAM] = std::make_shared<PBCombineSerializer>
		(PBContractParamSerializer::Instance(), PBStringTableSerializer::Instance());
	_serializer_map[MSG_ID_MODIFY_CONTRACT_PARAM] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBContractParamSerializer::Instance());
	_serializer_map[MSG_ID_MODIFY_STRATEGY] = PBStrategySerializer::Instance();
}

////////////////////////////////////////////////////////////////////////
// Name:       PBMessageSerializerFactory::~PBMessageSerializerFactory()
// Purpose:    Implementation of PBMessageSerializerFactory::~PBMessageSerializerFactory()
// Return:     
////////////////////////////////////////////////////////////////////////

PBMessageSerializerFactory::~PBMessageSerializerFactory()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       PBMessageSerializerFactory::CreateDataSerializers()
// Purpose:    Implementation of PBMessageSerializerFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

std::map<unsigned int, IDataSerializer_Ptr> PBMessageSerializerFactory::CreateDataSerializers(void)
{
	return _serializer_map;
}