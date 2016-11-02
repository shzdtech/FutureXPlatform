#include "TradeDAO.h"
#include "MySqlConnectionManager.h"

VectorDO_Ptr<TradeRecordDO> TradeDAO::QueryTrade(const std::string& userid, const ContractKey & contractKey, 
	const std::string& startDt, const std::string& endDt)
{
	static const std::string sql_proc_querytodayorder
	("CALL Trade_OTC_Query(?,?,?,?)");

	VectorDO_Ptr<TradeRecordDO> ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_querytodayorder));
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
			if (!ret)
				ret = std::make_shared<VectorDO<TradeRecordDO>>();

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
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

int TradeDAO::SaveExchangeTrade(int64_t tradeId, int64_t orderSysId, 
	const std::string& exchange, const std::string& contract, int quantity, double price,
	const std::string & tradingDay, const std::string & userid, const std::string & portfolio, bool isBuy, int openclose)
{
	int ret = 0;

	static const std::string sql_proc_savetrade("CALL Trade_Exchange(?,?,?,?,?,?,?,?,?,?,?)");

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
		prestmt->setDateTime(7, tradingDay);
		prestmt->setString(8, userid);
		prestmt->setString(9, portfolio);
		prestmt->setBoolean(10, isBuy);
		prestmt->setInt(11, openclose);

		prestmt->executeUpdate();
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.getSQLStateCStr();
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
		tradeDO.Direction != DirectionType::SELL, tradeDO.OpenClose);
}
