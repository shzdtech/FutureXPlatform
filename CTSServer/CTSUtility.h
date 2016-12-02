#pragma once
#include "../dataobject/OrderDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/AccountInfoDO.h"




class CTSUtility
{
public:
	static ::OrderStatusType CheckOrderStatus(T4::OrderStatus status, T4::OrderChange lastChange);
	static OrderDO_Ptr ParseRawOrder(T4::API::Order^ pOrder);
	static TradeRecordDO_Ptr ParseRawTrade(T4::API::Order::Trade^ pTrade);
	static UserPositionExDO_Ptr ParseRawPosition(T4::API::Position^ pPosition);
	static AccountInfoDO_Ptr ParseRawAccountInfo(T4::API::Account^ pAccount);

	
};

