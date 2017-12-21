/***********************************************************************
 * Module:  PBBizMessageSerializerFactory.cpp
 * Author:  milk
 * Modified: 2015年9月18日 10:26:49
 * Purpose: Implementation of the class PBBizMessageSerializerFactory
 ***********************************************************************/

#include "PBBizMessageSerializerFactory.h"
#include "pbbizserializers.h"
#include "../message/DefMessageID.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       PBBizMessageSerializerFactory::PBBizMessageSerializerFactory()
 // Purpose:    Implementation of PBBizMessageSerializerFactory::PBBizMessageSerializerFactory()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

PBBizMessageSerializerFactory::PBBizMessageSerializerFactory()
{
	//Market Data
	_serializer_map[MSG_ID_SUB_MARKETDATA] = PBSubMarketDataSerializer::Instance();
	_serializer_map[MSG_ID_UNSUB_MARKETDATA] = PBStringTableSerializer::Instance();
	_serializer_map[MSG_ID_RET_MARKETDATA] = PBMarketDataSerializer::Instance();

	//Trade
	_serializer_map[MSG_ID_ORDER_NEW] = PBOrderSerializer::Instance();
	_serializer_map[MSG_ID_ORDER_CANCEL] = PBOrderSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_ACCOUNT_INFO] = PBAccountInfoSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_EXCHANGE] = PBExchangeSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_INSTRUMENT] = PBContractInfoSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_ORDER] = std::make_shared<PBCombineSerializer>
		(PBOrderSerializer::Instance(), PBStringMapSerializer::Instance());
	_serializer_map[MSG_ID_ORDER_UPDATE] = PBOrderSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_POSITION] = PBUserPositionSerializer::Instance();
	_serializer_map[MSG_ID_POSITION_UPDATED] = PBUserPositionSerializer::Instance();
	_serializer_map[MSG_ID_EXCHANGE_POSITION_UPDATED] = PBUserPositionSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_TRADE] = std::make_shared<PBCombineSerializer>
		(PBTradeRecordSerializer::Instance(), PBStringMapSerializer::Instance());
	_serializer_map[MSG_ID_TRADE_RTN] = PBTradeRecordSerializer::Instance();
	_serializer_map[MSG_ID_SETTLEMENT_INFO_CONFIRM] = PBStringMapSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_POSITION_DIFFER] = std::make_shared<PBCombineSerializer>
		(PBPositionDifferSerializer::Instance(), PBStringMapSerializer::Instance());
	_serializer_map[MSG_ID_SYNC_POSITION] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBPositionDifferSerializer::Instance());
	_serializer_map[MSG_ID_ADD_MANUAL_TRADE] = PBTradeRecordSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_TRADE_DIFFER] = PBTradeRecordSerializer::Instance();
	_serializer_map[MSG_ID_SYNC_TRADE] = PBTradeRecordSerializer::Instance();

	//Transfer
	_serializer_map[MSG_ID_QUERY_TRANSFER_BANK] = PBBankSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_USER_BANKS] = PBUserAccountSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_TRANSFER_SERIAL] = PBBankOperationSerializer::Instance();
	_serializer_map[MSG_ID_REQ_BANK_TO_FUTURE] = PBBankOperationSerializer::Instance();
	_serializer_map[MSG_ID_REQ_FUTURE_TO_BANK] = PBBankOperationSerializer::Instance();
	_serializer_map[MSG_ID_RET_BANK_TO_FUTURE] = PBBankOperationSerializer::Instance();
	_serializer_map[MSG_ID_RET_FUTURE_TO_BANK] = PBBankOperationSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_USER_BANKACCOUNT] = PBUserAccountSerializer::Instance();

	//OTC
	_serializer_map[MSG_ID_SUB_PRICING] = PBSubPricingDataSerializer::Instance();
	_serializer_map[MSG_ID_RTN_PRICING] = PBPricingDataSerializer::Instance();
	_serializer_map[MSG_ID_UNSUB_PRICING] = PBStringTableSerializer::Instance();
	_serializer_map[MSG_ID_MODIFY_USER_PARAM] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBUserParamSerializer::Instance());
	_serializer_map[MSG_ID_QUERY_TRADINGDESK] = PBUserInfoListSerializer::Instance();
	_serializer_map[MSG_ID_QUERY_STRATEGY] = std::make_shared<PBCombineSerializer>
		(PBStrategyPricingContractSerializer::Instance(), PBStringMapSerializer::Instance());
	_serializer_map[MSG_ID_QUERY_CONTRACT_PARAM] = std::make_shared<PBCombineSerializer>
		(PBContractParamSerializer::Instance(), PBStringMapSerializer::Instance());
	_serializer_map[MSG_ID_MODIFY_CONTRACT_PARAM] = std::make_shared<PBCombineSerializer>
		(PBResultSerializer::Instance(), PBContractParamSerializer::Instance());
	_serializer_map[MSG_ID_MODIFY_STRATEGY] = PBStrategySerializer::Instance();
	_serializer_map[MSG_ID_QUERY_PORTFOLIO] = std::make_shared<PBCombineSerializer>
		(PBPortfolioListSerializer::Instance(), PBStringMapSerializer::Instance());
	_serializer_map[MSG_ID_PORTFOLIO_NEW] = PBPortfolioSerializer::Instance();
	_serializer_map[MSG_ID_MODIFY_PORTFOLIO] = PBPortfolioSerializer::Instance();
	_serializer_map[MSG_ID_HEDGE_CONTRACT_UPDATE] = PBPortfolioSerializer::Instance();
	_serializer_map[MSG_ID_RTN_TRADINGDESK_PRICING] = PBTradingDeskParamsSerializer::Instance();
	_serializer_map[MSG_ID_MODIFY_PRICING_CONTRACT] = PBStrategyPricingContractSerializer::Instance();
	_serializer_map[MSG_ID_SUB_TRADINGDESK_PRICING] = PBInstrumentSerializer::Instance();
	_serializer_map[MSG_ID_UNSUB_TRADINGDESK_PRICING] = PBInstrumentSerializer::Instance();

	_serializer_map[MSG_ID_QUERY_RISK] = _serializer_map[MSG_ID_RISK_UPDATED] =
		std::make_shared<PBCombineSerializer>(PBRiskSerializer::Instance(), PBStringMapSerializer::Instance());

	_serializer_map[MSG_ID_QUERY_VALUATION_RISK] = PBValuationRiskSerializer::Instance();
}

////////////////////////////////////////////////////////////////////////
// Name:       PBBizMessageSerializerFactory::~PBBizMessageSerializerFactory()
// Purpose:    Implementation of PBBizMessageSerializerFactory::~PBBizMessageSerializerFactory()
// Return:     
////////////////////////////////////////////////////////////////////////

PBBizMessageSerializerFactory::~PBBizMessageSerializerFactory()
{
	// TODO : implement
}

////////////////////////////////////////////////////////////////////////
// Name:       PBBizMessageSerializerFactory::CreateDataSerializers()
// Purpose:    Implementation of PBBizMessageSerializerFactory::CreateDataSerializers()
// Return:     std::map<uint, IDataSerializer_Ptr>
////////////////////////////////////////////////////////////////////////

void PBBizMessageSerializerFactory::CreateDataSerializers(std::map<uint32_t, IDataSerializer_Ptr>& serializerMap)
{
	serializerMap.insert(_serializer_map.begin(), _serializer_map.end());
}