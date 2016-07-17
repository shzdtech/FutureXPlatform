#include "TestingCancelOrderHandler.h"
#include "../dataobject/OrderDO.h"

dataobj_ptr TestingCancelOrderHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI * rawAPI, ISession * session)
{
	auto orderDO_ptr = std::make_shared<OrderDO>(*(OrderDO*)reqDO.get());

	if(std::rand() % 2)
	{
		orderDO_ptr->OrderStatus = OrderStatus::CANCELED;
		orderDO_ptr->Message = "Successfully cancel order";
	}
	else
	{
		orderDO_ptr->OrderStatus = OrderStatus::CANCEL_REJECTED;
		orderDO_ptr->Message = "Reject 'Cancel' by testing";
	}


	return orderDO_ptr;
}
