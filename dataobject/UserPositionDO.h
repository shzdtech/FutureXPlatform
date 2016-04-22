/***********************************************************************
 * Module:  UserPositionDO.h
 * Author:  milk
 * Modified: 2015年7月12日 15:10:23
 * Purpose: Declaration of the class UserPositionDO
 ***********************************************************************/

#if !defined(__dataobject_UserPositionDO_h)
#define __dataobject_UserPositionDO_h

#include "ContractKey.h"
#include "AbstractDataObj.h"
#include "EnumTypes.h"

class UserPositionDO : public ContractKey, public AbstractDataObj
{
public:
	UserPositionDO(const std::string& exchangeID, const std::string& instrumentID)
		: ContractKey(exchangeID, instrumentID)	{}

	int Direction = 0;
	int Position = 0;
	int LastdayPosition = 0;
	int OpenVolume = 0;
	int CloseVolume = 0;
	int HedgeFlag = 0;

	double OpenAmount = 0;
	double CloseAmount = 0;
	double Cost = 0;
	double OpenCost = 0;
	double Profit = 0;
	double CloseProfit = 0;
	double UseMargin = 0;
	
	std::string PositionDate;

protected:

private:

};

class UserPositionExDO : public UserPositionDO
{
public:
	UserPositionExDO(const std::string& exchangeID, const std::string& instrumentID)
		: UserPositionDO(exchangeID, instrumentID)	{}

	int SettlementID = 0;
	int LongFrozen = 0;
	int ShortFrozen = 0;

	double LongFrozenAmount = 0;
	double ShortFrozenAmount = 0;
	double SettlementPrice = 0;
	double PreSettlementPrice = 0;
	double ExchangeMargin = 0;
	double CashIn = 0;
	double CombPosition = 0;
	double CombLongFrozen = 0;
	double CombShortFrozen = 0;
	double Commission = 0;
	double CloseProfitByDate = 0;
	double CloseProfitByTrade = 0;
	double FrozenMargin = 0;
	double FrozenCash = 0;
	double FrozenCommission = 0;
	double MarginRateByMoney = 0;
	double MarginRateByVolume = 0;
	double PreMargin = 0;

	std::string TradingDay;
};

#endif