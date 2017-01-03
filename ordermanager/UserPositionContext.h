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
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/OrderDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/cuckoohashmap_wrapper.h"
#include "../utility/pairhash.h"

#include "ordermgr_export.h"


class ORDERMGR_CLASS_EXPORT UserPositionContext
{
public:
	void UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO, bool adjustPosition = true);
	void Clear(void);
	cuckoohash_map<std::string, cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>>>&
		UserPositionMap();
	cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>>
		GetPositionsByUser(const std::string& userID);
	UserPositionExDO_Ptr GetPosition(const std::string& userID, const std::string& instumentID, PositionDirectionType direction);

	bool RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction);

	void UpsertPosition(const TradeRecordDO_Ptr & tradeDO, PositionDirectionType pd, double cost);

	bool FreezePosition(const OrderRequestDO & orderRequestDO, int& todayVol, int& ydVol);

private:
	cuckoohash_map<std::string, cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>>> _userPositionMap;
};

#endif