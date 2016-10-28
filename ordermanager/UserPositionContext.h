/***********************************************************************
 * Module:  UserPositionContext..h
 * Author:  milk
 * Modified: 2015年10月28日 11:18:40
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_UserPositionContext_h)
#define __ordermanager_UserPositionContext_h

#include <atomic>
#include "../common/typedefs.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/UserPositionDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/cuckoohashmap_wrapper.h"
#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT UserPositionContext
{
public:
	void UpsertPosition(const std::string& userId, const UserPositionExDO_Ptr& positionDO_Ptr);
	void UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO);
	void Clear(void);
	cuckoohash_map<std::string, cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr>>& UserPositionMap();
	cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr> GetPositionsByUser(const std::string& userID);
	UserPositionExDO_Ptr GetPosition(const std::string& userID, const std::string& instumentID);

private:
	cuckoohash_map<std::string, cuckoohashmap_wrapper<std::string, UserPositionExDO_Ptr>> _userPositionMap;
};

#endif