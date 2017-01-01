/***********************************************************************
* Module:  PositionTradeDO.h
* Author:  milk
* Modified: 2015Äê7ÔÂ12ÈÕ 17:26:07
* Purpose: Declaration of the class PositionTradeDO
***********************************************************************/

#if !defined(__dataobject_PositionTradeDO_h)
#define __dataobject_PositionTradeDO_h

#include "ContractKey.h"
#include "dataobjectbase.h"
#include "OrderDO.h"

class PositionTradeDO
{
public:
	uint64_t TradeID = 0;
	double OpenPrice = 0;
	int Volume = 0;
	int TradingDay;

protected:

private:

};

typedef std::shared_ptr<PositionTradeDO> PositionTradeDO_Ptr;

#endif