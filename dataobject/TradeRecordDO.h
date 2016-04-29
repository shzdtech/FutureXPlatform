/***********************************************************************
 * Module:  TradeRecordDO.h
 * Author:  milk
 * Modified: 2015年7月12日 17:26:07
 * Purpose: Declaration of the class TradeRecordDO
 ***********************************************************************/

#if !defined(__dataobject_TradeRecordDO_h)
#define __dataobject_TradeRecordDO_h

#include "ContractKey.h"
#include "AbstractDataObj.h"

class TradeRecordDO : public ContractKey, public AbstractDataObj
{
public:
	TradeRecordDO(const std::string& exchangeID, const std::string& instrumentID)
		: ContractKey(exchangeID, instrumentID){}

	uint64_t OrderID = 0;
	uint64_t TradeID = 0;
	uint64_t OrderSysID = 0;

	double Price = 0;
	double Commission = 0;

	int Volume = 0;
	int Direction = 0;
	int TradeType = 0;
	int OpenClose = 0;
	int HedgeFlag = 0;

	std::string TradeDate;
	std::string TradeTime;

protected:

private:

};

#endif