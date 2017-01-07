/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserPositionContext.h"

void UserPositionContext::UpsertPosition(const std::string& userid, const UserPositionExDO& positionDO, AdjustDirectionType adjustPosition)
{
	if (!_userPositionMap.contains(userid))
		_userPositionMap.insert(userid, std::move(cuckoohashmap_wrapper<std::pair<std::string, int>,
			UserPositionExDO_Ptr, pairhash<std::string, int>>(true, 2)));

	auto newPosition_Ptr = std::make_shared<UserPositionExDO>(positionDO);
	newPosition_Ptr->TdPosition = 0;
	newPosition_Ptr->YdPosition = 0;
	newPosition_Ptr->OpenAmount = 0;
	newPosition_Ptr->OpenVolume = 0;
	newPosition_Ptr->CloseAmount = 0;
	newPosition_Ptr->CloseVolume = 0;

	_userPositionMap.update_fn(userid, [&newPosition_Ptr, adjustPosition](cuckoohashmap_wrapper<std::pair<std::string, int>,
		UserPositionExDO_Ptr, pairhash<std::string, int>>&positionMap)
	{
		positionMap.map()->upsert(std::pair<std::string, int>(newPosition_Ptr->InstrumentID(), newPosition_Ptr->Direction),
			[&newPosition_Ptr, adjustPosition](UserPositionExDO_Ptr& position_ptr)
		{
			position_ptr->PreSettlementPrice = newPosition_Ptr->PreSettlementPrice;
			position_ptr->SettlementPrice = newPosition_Ptr->SettlementPrice;
			position_ptr->CloseProfit = newPosition_Ptr->CloseProfit;
			position_ptr->CashIn = newPosition_Ptr->CashIn;
			position_ptr->UseMargin = newPosition_Ptr->UseMargin;
			position_ptr->TdCost = newPosition_Ptr->TdCost;
			position_ptr->TdProfit = newPosition_Ptr->TdProfit;
			position_ptr->YdCost = newPosition_Ptr->YdCost;
			position_ptr->YdProfit = newPosition_Ptr->YdProfit;

			if (adjustPosition < 0 &&
				position_ptr->TradingDay == newPosition_Ptr->TradingDay &&
				position_ptr->YdInitPosition != newPosition_Ptr->YdInitPosition &&
				position_ptr->YdInitPosition > 0)
			{
				int delta = position_ptr->CloseVolume - newPosition_Ptr->YdInitPosition;
				if (delta > 0)
				{
					position_ptr->YdPosition = -newPosition_Ptr->YdInitPosition;
					position_ptr->TdPosition = position_ptr->OpenVolume - delta;
				}
				else
				{
					position_ptr->YdPosition = -position_ptr->CloseVolume;
					position_ptr->TdPosition = position_ptr->OpenVolume;
				}

				if (position_ptr->LastPosition() < 0)
				{
					auto deltaPos = -position_ptr->LastPosition();
					position_ptr->YdPosition += deltaPos;
					position_ptr->TdPosition -= deltaPos;
				}
			}

			position_ptr->YdInitPosition = newPosition_Ptr->YdInitPosition;

		}, newPosition_Ptr);
	});
}

void UserPositionContext::Clear(void)
{
	_userPositionMap.clear();
}

cuckoohash_map<std::string, cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>>>& UserPositionContext::UserPositionMap()
{
	return _userPositionMap;
}

cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>> UserPositionContext::GetPositionsByUser(const std::string & userID)
{
	cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>> positionMap;
	_userPositionMap.find(userID, positionMap);

	return positionMap;
}


UserPositionExDO_Ptr UserPositionContext::GetPosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction)
{
	UserPositionExDO_Ptr ret;
	cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>> positionMap;
	if (_userPositionMap.find(userID, positionMap))
	{
		positionMap.map()->find(std::pair<std::string, int>(instumentID, direction), ret);
	}

	return ret;
}


bool UserPositionContext::RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction)
{
	bool ret = false;
	cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>> positionMap;
	if (_userPositionMap.find(userID, positionMap))
	{
		ret = positionMap.map()->erase(std::pair<std::string, int>(instumentID, direction));
	}

	return ret;
}

void UserPositionContext::UpsertPosition(const TradeRecordDO_Ptr& tradeDO, PositionDirectionType pd, InstrumentDO* pContractInfo, bool hasCloseToday)
{
	if (!_userPositionMap.contains(tradeDO->UserID()))
		_userPositionMap.insert(tradeDO->UserID(), std::move(cuckoohashmap_wrapper<std::pair<std::string, int>,
			UserPositionExDO_Ptr, pairhash<std::string, int>>(true, 2)));

	// cost
	double cost = tradeDO->Price * tradeDO->Volume;
	if (pContractInfo)
		cost *= pContractInfo->VolumeMultiple;

	// construct position
	UserPositionExDO_Ptr positionDO_Ptr(new UserPositionExDO(tradeDO->ExchangeID(), tradeDO->InstrumentID()));
	positionDO_Ptr->TradingDay = tradeDO->TradingDay;
	positionDO_Ptr->Direction = pd;
	positionDO_Ptr->HedgeFlag = tradeDO->HedgeFlag;

	if (tradeDO->OpenClose == OrderOpenCloseType::OPEN)
	{
		positionDO_Ptr->TdPosition = positionDO_Ptr->OpenVolume = tradeDO->Volume;
		positionDO_Ptr->OpenAmount = positionDO_Ptr->OpenCost = positionDO_Ptr->TdCost = cost;
	}
	else
	{
		if (tradeDO->OpenClose == OrderOpenCloseType::CLOSETODAY)
		{
			positionDO_Ptr->TdPosition = -tradeDO->Volume;
		}
		else
		{
			positionDO_Ptr->YdPosition = -tradeDO->Volume;
		}

		positionDO_Ptr->CloseVolume = tradeDO->Volume;
		positionDO_Ptr->CloseAmount = cost;
	}

	_userPositionMap.update_fn(tradeDO->UserID(), [&tradeDO, &positionDO_Ptr, hasCloseToday](cuckoohashmap_wrapper<std::pair<std::string, int>,
		UserPositionExDO_Ptr, pairhash<std::string, int>>&positionMap)
	{
		positionMap.map()->upsert(std::pair<std::string, int>(tradeDO->InstrumentID(), positionDO_Ptr->Direction),
			[&tradeDO, &positionDO_Ptr, hasCloseToday](UserPositionExDO_Ptr& position_ptr)
		{
			if (tradeDO->OpenClose == OrderOpenCloseType::OPEN)
			{
				position_ptr->TdPosition += positionDO_Ptr->OpenVolume;
				position_ptr->OpenVolume += positionDO_Ptr->OpenVolume;
				position_ptr->OpenAmount += positionDO_Ptr->OpenAmount;
				position_ptr->OpenCost += positionDO_Ptr->OpenAmount;
				position_ptr->TdCost += positionDO_Ptr->OpenAmount;
			}
			else
			{
				double tdMean = position_ptr->OpenVolume ? position_ptr->OpenAmount / position_ptr->OpenVolume : 0;

				if (hasCloseToday)
				{
					if (tradeDO->OpenClose == OrderOpenCloseType::CLOSETODAY)
					{
						position_ptr->TdPosition -= positionDO_Ptr->CloseVolume;
						position_ptr->TdCost = tdMean * position_ptr->TdPosition;
					}
					else
					{
						position_ptr->YdPosition -= positionDO_Ptr->CloseVolume;
						position_ptr->YdCost = position_ptr->PreSettlementPrice * position_ptr->LastPosition();
					}
				}
				else
				{
					if (position_ptr->LastPosition() > 0)
					{
						auto deltaPos = positionDO_Ptr->CloseVolume - position_ptr->LastPosition();
						if (deltaPos > 0)
						{
							position_ptr->TdPosition -= deltaPos;
							position_ptr->TdCost = tdMean * position_ptr->TdPosition;
							position_ptr->YdPosition -= position_ptr->LastPosition();
							position_ptr->YdCost = 0;
						}
						else
						{
							position_ptr->YdPosition -= positionDO_Ptr->CloseVolume;
							position_ptr->YdCost = position_ptr->PreSettlementPrice * position_ptr->LastPosition();
						}
					}
					else
					{
						position_ptr->TdPosition -= positionDO_Ptr->CloseVolume;
						position_ptr->TdCost = tdMean * position_ptr->TdPosition;
					}
				}

				position_ptr->CloseVolume += positionDO_Ptr->CloseVolume;
				position_ptr->CloseAmount += positionDO_Ptr->CloseAmount;
			}
		}, positionDO_Ptr);
	});
}

bool UserPositionContext::FreezePosition(const OrderRequestDO& orderRequestDO, int& todayVol, int& ydVol)
{
	bool ret = true;
	todayVol = orderRequestDO.Volume;
	ydVol = 0;
	if (orderRequestDO.OpenClose != OrderOpenCloseType::OPEN)
	{
		ret = ret && _userPositionMap.update_fn(orderRequestDO.UserID(),
			[&orderRequestDO, &ret, &todayVol, &ydVol](cuckoohashmap_wrapper<std::pair<std::string, int>,
				UserPositionExDO_Ptr, pairhash<std::string, int>>&positionMap)
		{
			PositionDirectionType pd = orderRequestDO.Direction == DirectionType::SELL ? PositionDirectionType::PD_LONG : PositionDirectionType::PD_SHORT;
			ret = ret &&  positionMap.map()->update_fn(std::pair<std::string, int>(orderRequestDO.InstrumentID(), pd),
				[&orderRequestDO, &ret, &todayVol, &ydVol](UserPositionExDO_Ptr& position_ptr)
			{
				if (position_ptr->Position() - position_ptr->FrozenVolume >= orderRequestDO.Volume)
				{
					int tdRemain = position_ptr->TdPosition - position_ptr->FrozenVolume;
					if (tdRemain > 0)
					{
						int remain = tdRemain - orderRequestDO.Volume;
						if (remain < 0)
						{
							todayVol = tdRemain;
							ydVol = -remain;
						}
					}
					else
					{
						ydVol = orderRequestDO.Volume;
					}

					position_ptr->FrozenVolume += orderRequestDO.Volume;
				}
				else
				{
					ret = false;
				}
			});
		});
	}

	return ret;
}