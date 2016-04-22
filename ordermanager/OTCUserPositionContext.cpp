/***********************************************************************
 * Module:  OTCUserOrderContext.cpp
 * Author:  milk
 * Modified: 2016年1月26日 17:31:32
 * Purpose: Implementation of the class OTCUserOrderContext
 ***********************************************************************/

#include "OTCUserPositionContext.h"
#include "../strategy/PricingContext.h"
#include "../utility/atomicutil.h"

int OTCUserPositionContext::UpdatePosition(
	const StrategyContractDO& strategyDO,
	DirectionType direction,
	OrderOpenCloseType openClose, 
	int newTraded)
{
	int pos = -1;

	if (newTraded > 0)
	{
		if (direction == DirectionType::SELL)
			newTraded = -newTraded;

		if (openClose != OrderOpenCloseType::OPEN)
			newTraded = -newTraded;

		pos = (_position.
			getorfill(strategyDO).
			getorfill<int>(strategyDO) += newTraded);
	}

	return pos;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCHedgeHandler::GenSpreadOrders(const PortfolioKey& portfolioKey)
// Purpose:    Implementation of OTCHedgeHandler::GenSpreadOrders()
// Parameters:
// - userID
// - portfolio
// Return:     VectorDO_Ptr<OrderDO>
////////////////////////////////////////////////////////////////////////

ContractMap<double> OTCUserPositionContext::GenSpreadPoints(
	const PortfolioKey& portfolioKey,
	bool updatePosition)
{
	ContractMap<double> hedgeMap;
	auto pStrategyMap = PricingContext::GetStrategyMap();

	double initPos = 0;
	double sumPos = 0;
	auto& positionMap = _position.getorfill(portfolioKey);
	for (auto &it : positionMap)
	{
		int position = it.second;
		if (position != 0)
		{
			auto& strategy = pStrategyMap->at(it.first);
			for (auto& bit : *strategy.BaseContracts)
			{
				double pos = bit.Weight*position;
				auto &mktpos = hedgeMap.getorfill(bit);
				atomicutil::round_add(mktpos, pos, EPSILON);
			}
		}
	}

	return hedgeMap;
}
