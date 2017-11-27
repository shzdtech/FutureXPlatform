#include "PositionDAO.h"
#include "MySqlConnectionManager.h"

#include "../dataobject/DateType.h"

VectorDO_Ptr<UserPositionExDO> PositionDAO::QueryLastDayPosition(const std::string & accountId, const std::string & sysuserId, const std::string & tradingDay)
{
	static const std::string sql_proc_queryExchangePosition("CALL Position_Exchange_Lastday(?,?)");

	VectorDO_Ptr<UserPositionExDO> ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_queryExchangePosition));
		prestmt->setString(1, sysuserId);
		prestmt->setDateTime(2, tradingDay);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		/* SELECT exchange, contract, portfolio_symbol, is_buy, lastday,`position`
			FROM exchange_position
			WHERE accountid = userid;*/

		while (rs->next())
		{
			if (!ret)
				ret = std::make_shared<VectorDO<UserPositionExDO>>();

			UserPositionExDO positionDO(rs->getString(1), rs->getString(2), rs->getString(3), accountId);
			positionDO.Direction = rs->getBoolean(4) ? PositionDirectionType::PD_LONG : PositionDirectionType::PD_SHORT;
			positionDO.TradingDay = DateType(rs->getString(5)).YYYYMMDD();
			positionDO.YdInitPosition = rs->getInt(6);

			ret->push_back(std::move(positionDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}


VectorDO_Ptr<UserPositionExDO> PositionDAO::QueryOTCLastDayPosition(const std::string & accountId, const std::string & tradingDay)
{
	static const std::string sql_proc_queryOTCPosition("CALL Position_OTC_Lastday(?,?)");

	VectorDO_Ptr<UserPositionExDO> ret;

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_queryOTCPosition));
		prestmt->setString(1, accountId);
		prestmt->setDateTime(2, tradingDay);

		AutoCloseResultSet_Ptr rs(prestmt->executeQuery());

		/* SELECT accountid, exchange, contract, portfolio_symbol, lastday,`position`
		FROM exchange_position
		WHERE accountid = userid;*/

		while (rs->next())
		{
			if (!ret)
				ret = std::make_shared<VectorDO<UserPositionExDO>>();

			UserPositionExDO positionDO(rs->getString(2), rs->getString(3), rs->getString(4), rs->getString(1));
			positionDO.TradingDay = DateType(rs->getString(5)).YYYYMMDD();
			positionDO.YdInitPosition = rs->getInt(6);

			ret->push_back(std::move(positionDO));
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}


bool PositionDAO::SyncPosition(const std::string& sysUserId, const std::vector<UserPositionDO>& positions)
{
	bool ret = false;

	static const std::string sql_proc_updateYdPosition = 
		"CALL Position_SyncSys(?,?,?,?,?,?,?)";

	auto session = MySqlConnectionManager::Instance()->LeaseOrCreate();

	try
	{
		AutoClosePreparedStmt_Ptr prestmt(
			session->getConnection()->prepareStatement(sql_proc_updateYdPosition));

		for (auto& pos : positions)
		{
			prestmt->setString(1, sysUserId);
			prestmt->setInt(2, pos.TradingDay);
			prestmt->setString(3, pos.ExchangeID());
			prestmt->setString(4, pos.InstrumentID());
			prestmt->setBoolean(5, pos.Direction == PositionDirectionType::PD_LONG);
			prestmt->setString(6, pos.PortfolioID());
			prestmt->setInt(7, pos.YdInitPosition);
			prestmt->executeUpdate();
		}
	}
	catch (sql::SQLException& sqlEx)
	{
		LOG_ERROR << __FUNCTION__ << ": " << sqlEx.what();
		throw DatabaseException(sqlEx.getErrorCode(), sqlEx.getSQLStateCStr());
	}

	return ret;
}