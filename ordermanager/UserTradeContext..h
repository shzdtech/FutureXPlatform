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
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/cuckoohashmap_wrapper.h"
#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT UserTradeContext
{
public:
	void AddTrade(const TradeRecordDO_Ptr& tradeDO_Ptr);
	void AddTrade(const TradeRecordDO& tradeDO);
	void Clear(void);
	TradeRecordDO_Ptr RemoveTrade(uint64_t orderID);
	cuckoohash_map<uint64_t, TradeRecordDO_Ptr>& GetAllTrade();
	cuckoohash_map<std::string, cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>>& UserTradeMap();
	vector_ptr<TradeRecordDO> GetTradesByUser(const std::string& userID);
	TradeRecordDO_Ptr FindTrade(uint64_t orderID);


private:
	cuckoohash_map<uint64_t, TradeRecordDO_Ptr> _tradeIdMap;
	cuckoohash_map<std::string, cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>> _userTradeMap;
};

#endif