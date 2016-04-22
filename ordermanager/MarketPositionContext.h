/***********************************************************************
 * Module:  MarketPositionContext.h
 * Author:  milk
 * Modified: 2016年2月4日 23:49:28
 * Purpose: Declaration of the class MarketPositionContext
 ***********************************************************************/

#if !defined(__ordermanager_MarketPositionContext_h)
#define __ordermanager_MarketPositionContext_h

#include "../dataobject/OrderDO.h"
#include "../dataobject/TypedefDO.h"
#include <atomic>

class MarketPositionContext
{
public:
	int UpdatePosition(const ContractKey& contractId, DirectionType direction,
		OrderOpenCloseType openClose, int deltaPos);

	int GetBuyPosition(const ContractKey& contractId);
	int GetSellPosition(const ContractKey& contractId);

protected:
	ContractMap<std::atomic_int> _buyPosition;
	ContractMap<std::atomic_int> _sellPosition;

private:

};

#endif