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

class ORDERMGR_CLASS_EXPORT UserTradeContext
{
public:
	UserTradeContext();

	bool InsertTrade(const TradeRecordDO_Ptr& tradeDO_Ptr);
	bool InsertTrade(const TradeRecordDO& tradeDO);
	void Clear(void);
	TradeRecordDO_Ptr RemoveTrade(uint64_t tradeID);
	cuckoohash_map<uint64_t, TradeRecordDO_Ptr>& GetAllTrade();
	cuckoohash_map<std::string, cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>>& UserTradeMap();
	cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr> GetTradesByUser(const std::string& userID);
	bool Contains(uint64_t tradeID);
	TradeRecordDO_Ptr FindTrade(uint64_t tradeID);


protected:
	cuckoohash_map<uint64_t, TradeRecordDO_Ptr> _tradeIdMap;
	cuckoohash_map<std::string, cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>> _userTradeMap;
};

#endif