/***********************************************************************
 * Module:  MarketPositionContext.cpp
 * Author:  milk
 * Modified: 2016年2月4日 23:49:28
 * Purpose: Implementation of the class MarketPositionContext
 ***********************************************************************/

#include "MarketPositionContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       MarketPositionContext::UpdatePosition()
// Purpose:    Implementation of MarketPositionContext::UpdatePosition()
// Return:     int
////////////////////////////////////////////////////////////////////////

int MarketPositionContext::UpdatePosition(
	const ContractKey& contractId,
	DirectionType direction,
	OrderOpenCloseType openClose, int deltaPos)
{
	if (openClose != OrderOpenCloseType::OPEN)
		deltaPos = -deltaPos;

	auto& pos = direction == DirectionType::SELL ?
		_sellPosition.getorfill<int>(contractId) : _buyPosition.getorfill<int>(contractId);

	pos += deltaPos;

	return pos;
}

int MarketPositionContext::GetBuyPosition(const ContractKey& contractId)
{
	return _buyPosition.getorfill<int>(contractId);
}

int MarketPositionContext::GetSellPosition(const ContractKey& contractId)
{
	return _sellPosition.getorfill<int>(contractId);
}
