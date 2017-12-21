/***********************************************************************
 * Module:  UserTradeContext..h
 * Author:  milk
 * Modified: 2015年10月28日 11:18:40
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_UserTradeContext_h)
#define __ordermanager_UserTradeContext_h

#include <atomic>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/UserPositionDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/cuckoohashmap_wrapper.h"
#include "ordermgr_export.h"

typedef cuckoohashmap_wrapper<uint128, TradeRecordDO_Ptr, UINT128Hash> UserTradeMapType;
typedef cuckoohash_map<std::string, UserTradeMapType> TradeMapType;
typedef cuckoohash_map<uint128, TradeRecordDO_Ptr, UINT128Hash> TradeIDMapType;

class ORDERMGR_CLASS_EXPORT UserTradeContext
{
public:
	UserTradeContext();

	bool InsertTrade(const std::string& userID, const TradeRecordDO_Ptr& tradeDO_Ptr);
	bool InsertTrade(const std::string& userID, const TradeRecordDO& tradeDO);
	void Clear(void);
	TradeRecordDO_Ptr RemoveTrade(uint128 tradeID);
	TradeIDMapType& GetAllTrade();
	TradeMapType& UserTradeMap();
	UserTradeMapType GetTradesByUser(const std::string& userID);
	bool Contains(uint128 tradeID);
	TradeRecordDO_Ptr FindTrade(uint128 tradeID);


protected:
	TradeIDMapType _tradeIdMap;
	TradeMapType _userTradeMap;
};

#endif