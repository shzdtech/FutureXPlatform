/***********************************************************************
 * Module:  OTCUserOrderContext.h
 * Author:  milk
 * Modified: 2016年1月26日 17:31:32
 * Purpose: Declaration of the class OTCUserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_OTCUserOrderContext_h)
#define __ordermanager_OTCUserOrderContext_h

#include <atomic>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"
#include "../utility/cuckoohashmap_wrapper.h"
#include "../pricingengine/IPricingDataContext.h"

class OTCUserPositionContext
{
public:
	OTCUserPositionContext(const IPricingDataContext_Ptr& pricingCtx);

	int UpdatePosition(const StrategyContractDO& strategyDO, DirectionType direction,
		OrderOpenCloseType openClose, int deltaPos);

	ContractMap<double> GenSpreadPoints(const PortfolioKey& portfolioKey);

protected:
	cuckoohash_map<PortfolioKey, cuckoohashmap_wrapper<ContractKey, int, ContractKeyHash>, PortfolioKeyHash> _position;
	IPricingDataContext_Ptr _pricingCtx;

private:

};

#endif