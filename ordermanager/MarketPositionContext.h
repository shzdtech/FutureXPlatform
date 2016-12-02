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
#include "../utility/cuckoohashmap_wrapper.h"

class MarketPositionContext
{
public:
	MarketPositionContext();

	int UpdatePosition(const ContractKey& contractId, DirectionType direction, int deltaPos);

	int GetPosition(const ContractKey& contractId);

protected:
	cuckoohash_map<ContractKey, int, ContractKeyHash> _contractPosition;

private:

};

#endif