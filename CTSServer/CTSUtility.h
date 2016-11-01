#pragma once
#include "../dataobject/OrderDO.h"

class CTSUtility
{
public:
	static ::OrderStatusType CheckOrderStatus(T4::OrderStatus status, T4::OrderChange lastChange);
	static OrderDO_Ptr ParseRawOrder(T4::API::Order^ pOrder);
};

