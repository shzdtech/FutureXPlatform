/***********************************************************************
 * Module:  OTCOrderDAO.cpp
 * Author:  milk
 * Modified: 2015年10月14日 14:58:04
 * Purpose: Implementation of the class OTCOrderDAO
 ***********************************************************************/

#include "OTCOrderDAO.h"
#include "MySqlConnectionManager.h"

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderDAO::CreateOrder(OrderDO& orderDO)
// Purpose:    Implementation of OTCOrderDAO::CreateOrder()
// Parameters:
// - orderDO
// Return:     bool
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr OTCOrderDAO::CreateOrder(const OrderRequestDO& orderDO, const IPricingDO& pricingDO)
{
	static const std::string sql_proc_createorder("CALL Order_OTC_New(?,?,?,?,?,?,?,?,?,?,@orderID,@orderSysID,@orderStatus)");

	auto ret = std::make_shared<OrderDO>(orderDO);

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_createorder));
		prestmt->setDouble(1, pricingDO.Bid().Price);
		prestmt->setDouble(2, pricingDO.Ask().Price);
		prestmt->setString(3, orderDO.UserID());
		prestmt->setString(4, orderDO.ExchangeID());
		prestmt->setString(5, orderDO.InstrumentID());
		prestmt->setDouble(6, orderDO.LimitPrice);
		prestmt->setInt(7, orderDO.Volume);
		prestmt->setBoolean(8, orderDO.Direction != DirectionType::SELL);
		prestmt->setInt(9, orderDO.ExecType);
		prestmt->setInt(10, orderDO.TIF);

		prestmt->execute();

		AutoCloseStatement_Ptr stmt(session->getConnection()->createStatement());
		AutoCloseResultSet_Ptr rsout(stmt->executeQuery("select @orderID,@orderSysID,@orderStatus"));
		if (rsout->next())
		{
			ret->OrderID = rsout->getUInt64(1);
			ret->OrderSysID = rsout->getUInt64(2);
			ret->OrderStatus = (OrderStatusType)rsout->getInt(3);
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderDAO::CancelOrder(OrderDO& orderDO)
// Purpose:    Implementation of OTCOrderDAO::CancelOrder()
// Parameters:
// - orderDO
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool OTCOrderDAO::CancelOrder(const OrderRequestDO& orderDO, OrderStatusType& status)
{
	static const std::string sql_proc_cancelorder
	("CALL Order_OTC_Cancel(?,?)");

	bool ret = true;
	status = OrderStatusType::UNDEFINED;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_cancelorder));
		prestmt->setUInt64(1, orderDO.OrderID);
		prestmt->setInt(2, orderDO.ErrorCode);

		if (prestmt->executeUpdate() > 0)
		{
			ret = status == OrderStatusType::CANCELED;
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderDAO::AcceptOrder(OrderDO& orderDO)
// Purpose:    Implementation of OTCOrderDAO::AcceptOrder()
// Parameters:
// - orderDO
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool OTCOrderDAO::AcceptOrder(const OrderRequestDO& orderDO, OrderStatusType& status)
{
	static const std::string sql_proc_acceptorder("CALL Order_OTC_Accept(?)");

	bool ret = true;
	status = OrderStatusType::UNDEFINED;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_acceptorder));
		prestmt->setUInt64(1, orderDO.OrderID);

		if (prestmt->executeUpdate() > 0)
		{
			ret = status == OrderStatusType::ALL_TRADED;
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderDAO::AcceptOrder(OrderDO& orderDO)
// Purpose:    Implementation of OTCOrderDAO::AcceptOrder()
// Parameters:
// - orderDO
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool OTCOrderDAO::RejectOrder(const OrderRequestDO& orderDO, OrderStatusType& status)
{
	static const std::string sql_proc_rejectorder("CALL Order_OTC_Reject(?)");

	bool ret = true;
	status = OrderStatusType::UNDEFINED;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_rejectorder));
		prestmt->setUInt64(1, orderDO.OrderID);

		if (prestmt->executeUpdate() > 0)
		{
			ret = status == OrderStatusType::REJECTED;
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderDAO::QueryTradingOrder(const ContractKey& contractKey)
// Purpose:    Implementation of OTCOrderDAO::QueryTradingOrder()
// Parameters:
// - contractKey
// Return:     VectorDO_Ptr<OrderDO>
////////////////////////////////////////////////////////////////////////

OrderDOVec_Ptr OTCOrderDAO::QueryTradingOrder(const ContractKey& contractKey)
{
	static const std::string sql_proc_querytradingorder
	("CALL Order_OTC_QueryActive(?)");
	
	OrderDOVec_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_querytradingorder));
		prestmt->setString(1, contractKey.InstrumentID());

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		ret = std::make_shared<VectorDO<OrderDO>>();
		while (rs->next())
		{
			OrderDO obDO(rs->getUInt64(1), rs->getString(2), rs->getString(3), rs->getString(4), rs->getString(13));
			obDO.OrderSysID = rs->isNull(5) ? 0 : rs->getUInt64(5);
			obDO.LimitPrice = rs->getDouble(6);
			obDO.Volume = rs->getInt(7);
			obDO.VolumeTraded = rs->getInt(8);
			obDO.Direction = rs->getBoolean(9) ? DirectionType::BUY : DirectionType::SELL;
			obDO.OrderStatus = (OrderStatusType)rs->getInt(10);
			obDO.TIF = (OrderTIFType)rs->getInt(11);
			obDO.TradingType = (OrderTradingType)rs->getInt(12);
			obDO.InsertTime = rs->getString(14);
			ret->push_back(std::move(obDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderDAO::QueryTradingOrder(const ContractKey& contractKey)
// Purpose:    Implementation of OTCOrderDAO::QueryTradingOrder()
// Parameters:
// - contractKey
// Return:     VectorDO_Ptr<OrderDO>
////////////////////////////////////////////////////////////////////////

OrderDOVec_Ptr OTCOrderDAO::QueryTodayOrder(const std::string& userId, const ContractKey& contractKey)
{
	static const std::string sql_proc_querytodayorder
	("CALL Order_OTC_QueryToday(?,?)");

	OrderDOVec_Ptr ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_querytodayorder));
		prestmt->setString(1, userId);
		prestmt->setString(2, contractKey.InstrumentID());

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			if (!ret)
				ret = std::make_shared<VectorDO<OrderDO>>();

			/*SELECT `id`, `exchange`, `contract`, `accountid`, `sysid`, `price`, `quantity`, `quantity_filled`,
				`is_buy`, `status`, `tif`, `trading_type`, `portfolio_symbol`, DATE_FORMAT(create_time, '%H:%i:%s') AS createtime
				FROM order_record*/

			OrderDO obDO(rs->getUInt64(1), rs->getString(2), rs->getString(3), rs->getString(4), rs->getString(13));
			obDO.OrderSysID = rs->isNull(5) ? 0 : rs->getUInt64(5);
			obDO.LimitPrice = rs->getDouble(6);
			obDO.Volume = rs->getInt(7);
			obDO.VolumeTraded = rs->getInt(8);
			obDO.Direction = rs->getBoolean(9) != 0 ? DirectionType::BUY : DirectionType::SELL;
			obDO.OrderStatus = (OrderStatusType)rs->getInt(10);
			obDO.TIF = (OrderTIFType)rs->getInt(11);
			obDO.TradingType = (OrderTradingType)rs->getInt(12);
			obDO.InsertTime = rs->getString(14);
			ret->push_back(std::move(obDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}