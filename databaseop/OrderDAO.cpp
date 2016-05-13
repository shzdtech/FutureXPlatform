/***********************************************************************
 * Module:  OrderDAO.cpp
 * Author:  milk
 * Modified: 2015年10月16日 0:28:29
 * Purpose: Implementation of the class OrderDAO
 ***********************************************************************/

#include "OrderDAO.h"
#include "ConnectionHelper.h"
#include <glog/logging.h>

////////////////////////////////////////////////////////////////////////
// Name:       OrderDAO::CreateOrder(const OrderDO& orderDO)
// Purpose:    Implementation of OrderDAO::CreateOrder()
// Parameters:
// - orderDO
// Return:     bool
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr OrderDAO::CreateOrder(const OrderDO& orderDO)
{
	static const std::string sql_proc_createorder("CALL Order_Auto_New(?,?,?,?,?,?,@orderID)");

	OrderDO_Ptr ret;

	auto session = ConnectionHelper::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_createorder));
		prestmt->setString(1, orderDO.UserID());
		prestmt->setString(2, orderDO.ExchangeID());
		prestmt->setString(3, orderDO.InstrumentID());
		prestmt->setDouble(4, orderDO.LimitPrice);
		prestmt->setInt(5, orderDO.Volume);
		prestmt->setInt(6, orderDO.Direction);

		prestmt->execute();

		ret = std::make_shared<OrderDO>(orderDO);
		AutoCloseStatement_Ptr stmt(session->getConnection()->createStatement());
		AutoCloseResultSet_Ptr rsout(stmt->executeQuery("select @orderID"));
		if (rsout->next())
		{
			ret->OrderID = rsout->getUInt64(1);
		}
	}
	catch (std::exception& ex)
	{
		LOG(ERROR) << __FUNCTION__ << ": " << ex.what();
	}

	return ret;
}