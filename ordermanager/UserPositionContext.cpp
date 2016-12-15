/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserPositionContext.h"

void UserPositionContext::UpsertPosition(const std::string& userid, const UserPositionExDO_Ptr & positionDO_Ptr)
{
	if (!_userPositionMap.contains(userid))
		_userPositionMap.insert(userid, std::move(cuckoohashmap_wrapper<std::pair<std::string, int>,
			UserPositionExDO_Ptr, pairhash<std::string, int>>(true, 2)));

	_userPositionMap.update_fn(userid, [&positionDO_Ptr](cuckoohashmap_wrapper<std::pair<std::string, int>, 
		UserPositionExDO_Ptr, pairhash<std::string, int>>& positionMap)
	{
		positionMap.map()->upsert(std::pair<std::string, int>(positionDO_Ptr->InstrumentID(), positionDO_Ptr->Direction),
			[&positionDO_Ptr](UserPositionExDO_Ptr& position_ptr)
		{
			*position_ptr = *positionDO_Ptr;
		}, positionDO_Ptr);
	});
}

void UserPositionContext::UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO)
{
	UpsertPosition(userId, std::make_shared<UserPositionExDO>(positionDO));
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

UserPositionExDO_Ptr UserPositionContext::UpsertPosition(const TradeRecordDO_Ptr& tradeDO, PositionDirectionType pd, double cost)
{
	if (!_userPositionMap.contains(tradeDO->UserID()))
		_userPositionMap.insert(tradeDO->UserID(), std::move(cuckoohashmap_wrapper<std::pair<std::string, int>,
			UserPositionExDO_Ptr, pairhash<std::string, int>>(true, 2)));

	//construct position
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
		positionDO_Ptr->CloseVolume = tradeDO->Volume;
		positionDO_Ptr->CloseAmount = cost;
		if (tradeDO->OpenClose == OrderOpenCloseType::CLOSEYESTERDAY)
		{
			positionDO_Ptr->YdPosition = tradeDO->Volume;
		}
		else
		{
			positionDO_Ptr->TdPosition = tradeDO->Volume;
		}
	}

	_userPositionMap.update_fn(tradeDO->UserID(), [&tradeDO, &positionDO_Ptr](cuckoohashmap_wrapper<std::pair<std::string, int>,
		UserPositionExDO_Ptr, pairhash<std::string, int>>&positionMap)
	{
		positionMap.map()->upsert(std::pair<std::string, int>(tradeDO->InstrumentID(), positionDO_Ptr->Direction),
			[&tradeDO, &positionDO_Ptr](UserPositionExDO_Ptr& position_ptr)
		{
			if (tradeDO->OpenClose == OrderOpenCloseType::OPEN)
			{
				position_ptr->TdPosition += positionDO_Ptr->TdPosition;
				position_ptr->OpenVolume += positionDO_Ptr->OpenVolume;
				position_ptr->OpenAmount += positionDO_Ptr->OpenAmount;
				position_ptr->OpenCost += positionDO_Ptr->OpenCost;
				position_ptr->TdCost += positionDO_Ptr->TdCost;
			}
			else
			{
				double meanCost = position_ptr->Position() ? position_ptr->Cost() / position_ptr->Position() : 0;
				if (positionDO_Ptr->Position() > position_ptr->YdPosition)
				{
					position_ptr->TdPosition -= (positionDO_Ptr->Position() - position_ptr->YdPosition);
					position_ptr->TdCost = meanCost * position_ptr->TdPosition;
					position_ptr->YdPosition = 0;
					position_ptr->YdCost = 0;
				}
				else
				{
					position_ptr->YdPosition -= positionDO_Ptr->Position();
					position_ptr->YdCost = meanCost * position_ptr->YdPosition;
				}
				
				position_ptr->CloseVolume += positionDO_Ptr->CloseVolume;
				position_ptr->CloseAmount += positionDO_Ptr->CloseAmount;

				// copy for return
				positionDO_Ptr = position_ptr;
			}
		}, positionDO_Ptr);
	});

	return positionDO_Ptr;
}