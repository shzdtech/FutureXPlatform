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
			UserPositionExDO_Ptr, pairhash<std::string, int>>(true)));

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