#include "MarketDataDAO.h"
#include "MySqlConnectionManager.h"

bool MarketDataDAO::SaveMarketData(const MarketDataDO & marketDataDO)
{
	static const std::string sql_savemarketdata(
		"INSERT INTO market_data (exchange, contract, tradingday, presettleprice, settleprice) "
		"VALUES (?,?,?,?,?) ON DUPLICATE KEY UPDATE settleprice = ?") ;

	auto ret = -1;
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_savemarketdata));
		prestmt->setString(1, marketDataDO.ExchangeID());
		prestmt->setString(2, marketDataDO.InstrumentID());
		prestmt->setInt(3, marketDataDO.TradingDay);
		prestmt->setDouble(4, marketDataDO.PreSettlementPrice);
		prestmt->setDouble(5, marketDataDO.SettlementPrice);
		prestmt->setDouble(6, marketDataDO.SettlementPrice);

		prestmt->executeUpdate();

		ret = 0;
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}

vector_ptr<MarketDataDO> MarketDataDAO::LoadMarketData(int tradingDay)
{
	static const std::string sql_findportfolio(
		"SELECT exchange, contract, presettleprice, settleprice FROM market_data where tradingday=?");

	auto ret = std::make_shared<std::vector<MarketDataDO>>();
	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();
	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_findportfolio));
		prestmt->setInt(1, tradingDay);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		while (rs->next())
		{
			MarketDataDO mdo(rs->getString(1), rs->getString(2));
			mdo.PreSettlementPrice = rs->getDouble(3);
			mdo.SettlementPrice = rs->getDouble(4);

			ret->push_back(std::move(mdo));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}
