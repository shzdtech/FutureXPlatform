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
		_userPositionMap.insert(userid, std::move(cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr>(true)));

	_userPositionMap.update_fn(userid, [&positionDO_Ptr](cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr>& positionMap)
	{
		positionMap.map()->upsert(positionDO_Ptr->InstrumentID(), [&positionDO_Ptr](UserPositionExDO_Ptr& position_ptr)
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

cuckoohash_map<std::string, cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr>>& UserPositionContext::UserPositionMap()
{
	return _userPositionMap;
}

cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr> UserPositionContext::GetPositionsByUser(const std::string & userID)
{
	cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr> positionMap;
	_userPositionMap.find(userID, positionMap);

	return positionMap;
}

UserPositionExDO_Ptr UserPositionContext::GetPosition(const std::string & userID, const std::string & instumentID)
{
	UserPositionExDO_Ptr ret;
	cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr> positionMap;
	if (_userPositionMap.find(userID, positionMap))
	{
		positionMap.map()->find(instumentID, ret);
	}

	return ret;
}
