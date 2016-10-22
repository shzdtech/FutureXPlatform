/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserTradeContext.h"

void UserTradeContext::UpsertTrade(const TradeRecordDO_Ptr & tradeDO_Ptr)
{
	_tradeIdMap.upsert(tradeDO_Ptr->TradeID, [&tradeDO_Ptr](TradeRecordDO_Ptr& tradeptr)
	{
		*tradeptr = *tradeDO_Ptr;
	}, tradeDO_Ptr);

	if (!_userTradeMap.contains(tradeDO_Ptr->UserID()))
		_userTradeMap.insert(tradeDO_Ptr->UserID(), std::move(cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>(true)));

	_userTradeMap.update_fn(tradeDO_Ptr->UserID(), [this, &tradeDO_Ptr](cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>& tradeMap)
	{
		if (!tradeMap.map()->contains(tradeDO_Ptr->TradeID))
		{
			if (auto instoreptr = FindTrade(tradeDO_Ptr->TradeID))
			{
				tradeMap.map()->insert(tradeDO_Ptr->TradeID, instoreptr);
			}
		}
	});
}

void UserTradeContext::UpsertTrade(const TradeRecordDO& tradeDO)
{
	UpsertTrade(std::make_shared<TradeRecordDO>(tradeDO));
}

void UserTradeContext::Clear(void)
{
	_userTradeMap.clear();
	_tradeIdMap.clear();
}

TradeRecordDO_Ptr UserTradeContext::RemoveTrade(uint64_t tradeID)
{
	TradeRecordDO_Ptr ret;
	if (_tradeIdMap.find(tradeID, ret))
	{
		_tradeIdMap.erase(tradeID);

		cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr> tradeMap;
		if(_userTradeMap.find(ret->UserID(), tradeMap))
		{
			tradeMap.map()->erase(tradeID);
		}
	}
	return ret;
}

cuckoohash_map<uint64_t, TradeRecordDO_Ptr>& UserTradeContext::GetAllTrade()
{
	return _tradeIdMap;
}

cuckoohash_map<std::string, cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>>& UserTradeContext::UserTradeMap()
{
	return _userTradeMap;
}

cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr> UserTradeContext::GetTradesByUser(const std::string & userID)
{
	cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr> ret;
	_userTradeMap.find(userID, ret);

	return ret;
}


TradeRecordDO_Ptr UserTradeContext::FindTrade(uint64_t tradeID)
{
	TradeRecordDO_Ptr ret;
	_tradeIdMap.find(tradeID, ret);

	return ret;
}