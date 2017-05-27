#include "TradeDAO.h"
#include "../dataobject/DateType.h"
#include "MySqlConnectionManager.h"

VectorDO_Ptr<TradeRecordDO> TradeDAO::QueryOTCUserTrades(const std::string& userid, const ContractKey & contractKey, 
	const std::string& startDt, const std::string& endDt)
{
	static const std::string sql_proc_queryOTCTrade
	("CALL Trade_OTC_Query(?,?,?,?)");

	auto ret = std::make_shared<VectorDO<TradeRecordDO>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_queryOTCTrade));
		prestmt->setString(1, userid);
		prestmt->setString(2, contractKey.InstrumentID());
		prestmt->setDateTime(3, startDt);
		prestmt->setDateTime(4, endDt);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		/*SELECT b.exchange, b.contract, a.id AS tradeId, a.price, a.quantity,
			DATE_FORMAT(a.`trade_timestamp`, '%Y%m%d') AS tradeDate, DATE_FORMAT(a.`trade_timestamp`, '%H:%i:%s') AS tradeTime,
			b.is_buy, b.id AS orderId, b.sysid AS orderSysId*/

		while (rs->next())
		{
			TradeRecordDO tradeDO(rs->getString(1), rs->getString(2), userid, "");
			tradeDO.TradeID = rs->getUInt64(3);
			tradeDO.Price = rs->getDouble(4);
			tradeDO.Volume = rs->getInt(5);
			tradeDO.TradeDate = rs->getString(6);
			tradeDO.TradeTime = rs->getString(7);
			tradeDO.Direction = rs->getBoolean(8) ? DirectionType::BUY : DirectionType::SELL;
			tradeDO.OrderID = rs->getUInt64(9);
			tradeDO.OrderSysID = rs->getUInt64(10);

			ret->push_back(std::move(tradeDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

VectorDO_Ptr<TradeRecordDO> TradeDAO::QueryExchangeTrade(const std::string & sysuserid, const std::string& contract, 
	const std::string & startDt, const std::string & endDt)
{
	static const std::string sql_proc_queryExchangeTrade
	("SELECT tradeid,ordersysid,exchange,contract,quantity, "
		"price,tradingday,portfolio_symbol,is_buy,openclose,accountid, "
		"DATE_FORMAT(insert_time, '%Y%m%d') AS tradeDate, DATE_FORMAT(insert_time, '%H:%i:%s') AS tradeTime "
		"FROM exchange_trade WHERE sysuserid=? AND contract like ? AND tradingday >= ? AND tradingday <= ?");

	auto ret = std::make_shared<VectorDO<TradeRecordDO>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_queryExchangeTrade));
		prestmt->setString(1, sysuserid);
		prestmt->setString(2, contract.empty() ? "%" : contract);
		prestmt->setDateTime(3, startDt);
		prestmt->setDateTime(4, endDt);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		/*SELECT b.exchange, b.contract, a.id AS tradeId, a.price, a.quantity,
		DATE_FORMAT(a.`trade_timestamp`, '%Y%m%d') AS tradeDate, DATE_FORMAT(a.`trade_timestamp`, '%H:%i:%s') AS tradeTime,
		b.is_buy, b.id AS orderId, b.sysid AS orderSysId*/

		while (rs->next())
		{
			TradeRecordDO tradeDO(rs->getString(3), rs->getString(4), rs->getString(11), rs->getString(8));
			tradeDO.TradeID = rs->getUInt64(1);
			tradeDO.OrderSysID = rs->getDouble(2);
			tradeDO.Volume = rs->getInt(5);
			tradeDO.Price = rs->getDouble(6);
			tradeDO.TradingDay = DateType(rs->getString(7)).YYYYMMDD();
			tradeDO.Direction = rs->getBoolean(9) ? DirectionType::BUY : DirectionType::SELL;
			tradeDO.OpenClose = (OrderOpenCloseType)rs->getInt(10);
			tradeDO.TradeDate = rs->getString(12);
			tradeDO.TradeTime = rs->getString(13);

			tradeDO.InvestorID = sysuserid;

			ret->push_back(std::move(tradeDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

VectorDO_Ptr<TradeRecordDO> TradeDAO::QueryAllExchangeTrades(const std::string & startDt, const std::string & endDt)
{
	static const std::string sql_proc_queryExchangeTrade
	("SELECT tradeid,ordersysid,exchange,contract,quantity, "
		"price,tradingday,portfolio_symbol,is_buy,openclose, "
		"accountid, sysuserid "
		"FROM exchange_trade WHERE tradingday >= ? AND tradingday <= ?");

	auto ret = std::make_shared<VectorDO<TradeRecordDO>>();

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_queryExchangeTrade));
		prestmt->setDateTime(1, startDt);
		prestmt->setDateTime(2, endDt);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		/*SELECT b.exchange, b.contract, a.id AS tradeId, a.price, a.quantity,
		DATE_FORMAT(a.`trade_timestamp`, '%Y%m%d') AS tradeDate, DATE_FORMAT(a.`trade_timestamp`, '%H:%i:%s') AS tradeTime,
		b.is_buy, b.id AS orderId, b.sysid AS orderSysId*/

		while (rs->next())
		{
			TradeRecordDO tradeDO(rs->getString(3), rs->getString(4), rs->getString(11), rs->getString(8));
			tradeDO.TradeID = rs->getUInt64(1);
			tradeDO.OrderSysID = rs->getDouble(2);
			tradeDO.Volume = rs->getInt(5);
			tradeDO.Price = rs->getDouble(6);
			tradeDO.TradingDay = DateType(rs->getString(7)).YYYYMMDD();
			tradeDO.SetPortfolioID(rs->getString(8));
			tradeDO.Direction = rs->getBoolean(9) ? DirectionType::BUY : DirectionType::SELL;
			tradeDO.OpenClose = (OrderOpenCloseType)rs->getInt(10);
			tradeDO.InvestorID = rs->getString(12);

			ret->push_back(std::move(tradeDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

int TradeDAO::SaveExchangeTrade(int64_t tradeId, int64_t orderSysId, 
	const std::string& exchange, const std::string& contract, int quantity, double price,
	int tradingDay, const std::string & userid, const std::string & portfolio, bool isBuy, int openclose,
	const std::string& sysuserid)
{
	static const std::string sql_proc_savetrade("CALL Trade_Exchange(?,?,?,?,?,?,?,?,?,?,?,?)");

	int ret = 0;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_savetrade));
		prestmt->setUInt64(1, tradeId);
		prestmt->setUInt64(2, orderSysId);
		prestmt->setString(3, exchange);
		prestmt->setString(4, contract);
		prestmt->setUInt(5, quantity);
		prestmt->setDouble(6, price);
		prestmt->setDateTime(7, std::to_string(tradingDay));
		prestmt->setString(8, userid);
		prestmt->setString(9, portfolio);
		prestmt->setBoolean(10, isBuy);
		prestmt->setInt(11, openclose);
		prestmt->setString(12, sysuserid);

		prestmt->executeUpdate();
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		ret = sqlEx.getErrorCode();
	}

	return 0;
}

int TradeDAO::SaveExchangeTrade(const TradeRecordDO & tradeDO)
{
	return SaveExchangeTrade(tradeDO.TradeID, tradeDO.OrderSysID,
		tradeDO.ExchangeID(), tradeDO.InstrumentID(),
		tradeDO.Volume, tradeDO.Price,
		tradeDO.TradingDay, tradeDO.UserID(), tradeDO.PortfolioID(),
		tradeDO.Direction != DirectionType::SELL, tradeDO.OpenClose, tradeDO.InvestorID);
}
