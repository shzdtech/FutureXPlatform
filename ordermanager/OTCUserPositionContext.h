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
#include "../pricingengine/IPricingDataContext.h"

class OTCUserPositionContext
{
public:
	OTCUserPositionContext(IPricingDataContext* pricingCtx);

	int UpdatePosition(const StrategyContractDO& strategyDO, DirectionType direction,
		OrderOpenCloseType openClose, int deltaPos);

	ContractMap<double> GenSpreadPoints(const PortfolioKey& portfolioKey,
		bool updatePosition);

protected:
	PortfolioMap<ContractMap<std::atomic_int>> _position;
	IPricingDataContext* _pricingCtx;

private:

};

#endif