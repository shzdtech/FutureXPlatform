/***********************************************************************
 * Module:  AccountInfoDO.h
 * Author:  milk
 * Modified: 2015年7月12日 14:15:48
 * Purpose: Declaration of the class AccountInfoDO
 ***********************************************************************/

#if !defined(__dataobject_AccountInfoDO_h)
#define __dataobject_AccountInfoDO_h

#include "AbstractDataObj.h"

class AccountInfoDO : public AbstractDataObj
{
public:
	std::string AccountID;
	std::string BrokerID;
	std::string TradingDay;

	int SettlementID = 0;
	double PreMortgage = 0;
	double PreCredit = 0;
	double PreDeposit = 0;
	double PreBalance = 0;
	double PreMargin = 0;
	double InterestBase = 0;
	double Interest = 0;
	double Deposit = 0;
	double Withdraw = 0;
	double FrozenMargin = 0;
	double FrozenCash = 0;
	double FrozenCommission = 0;
	double CurrMargin = 0;
	double CashIn = 0;
	double Commission = 0;
	double CloseProfit = 0;
	double PositionProfit = 0;
	double Available = 0;
	double Balance = 0;
	double WithdrawQuota = 0;
	double Reserve = 0;
	double Credit = 0;
	double Mortgage = 0;
	double ExchangeMargin = 0;
	double DeliveryMargin = 0;
	double ExchangeDeliveryMargin = 0;
	double ReserveBalance = 0;

protected:

private:

};

#endif