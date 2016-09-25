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
	DirectionType direction, int deltaPos)
{
	int updatedPos = 0;

	if (deltaPos > 0 && direction == DirectionType::SELL) deltaPos = -deltaPos;

	_contractPosition.upsert(contractId, [&](int& pos)
	{
		pos += deltaPos;
		updatedPos = pos;
	}, deltaPos);

	return updatedPos;
}

int MarketPositionContext::GetPosition(const ContractKey & contractId)
{
	int position;
	_contractPosition.find(contractId, position);

	return position;
}
