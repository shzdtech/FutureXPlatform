/***********************************************************************
 * Module:  OTCUserOrderContext.cpp
 * Author:  milk
 * Modified: 2016年1月26日 17:31:32
 * Purpose: Implementation of the class OTCUserOrderContext
 ***********************************************************************/

#include "OTCUserPositionContext.h"
#include "../utility/atomicutil.h"
#include "../utility/epsdouble.h"

OTCUserPositionContext::OTCUserPositionContext(const IPricingDataContext_Ptr & pricingCtx)
	: _pricingCtx(pricingCtx), _position(16)
{
}

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

		while (!_position.update_fn(strategyDO, [&](cuckoohashmap_wrapper<ContractKey, int, ContractKeyHash>& contractPos)
		{
			contractPos.map()->update_fn(strategyDO, [&](int& position)
			{
				pos = (position += newTraded);
			});
		}))
		{
			cuckoohashmap_wrapper<ContractKey, int, ContractKeyHash> contractPos;
			contractPos.map()->insert(strategyDO, 0);
			_position.insert(strategyDO, contractPos);
		}
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

ContractMap<double> OTCUserPositionContext::GenSpreadPoints(const PortfolioKey& portfolioKey)
{
	ContractMap<double> hedgeMap;
	auto pStrategyMap = _pricingCtx->GetStrategyMap();

	double initPos = 0;
	double sumPos = 0;

	_position.update_fn(portfolioKey, [&](cuckoohashmap_wrapper<ContractKey, int, ContractKeyHash>& contractPos)
	{
		static const double EPSILON = 1e-6;
		auto lt = contractPos.map()->lock_table();
		for (auto &it : lt)
		{
			int position = it.second;
			if (position != 0)
			{
				if (auto pStrategy = pStrategyMap->tryfind(it.first))
				{
					for (auto& bit : pStrategy->PricingContracts->PricingContracts)
					{
						double pos = bit.Weight*position;
						auto &mktpos = hedgeMap.getorfill(bit);
						atomicutil::round_add(mktpos, pos, EPSILON);
					}
				}
			}
		}
	});

	return hedgeMap;
}
