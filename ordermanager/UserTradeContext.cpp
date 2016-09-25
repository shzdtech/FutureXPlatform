/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserTradeContext..h"

void UserTradeContext::AddTrade(const TradeRecordDO_Ptr & tradeDO_Ptr)
{
	if (!_userTradeMap.contains(tradeDO_Ptr->UserID()))
		_userTradeMap.insert(tradeDO_Ptr->UserID(), std::move(cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>{}));

	_userTradeMap.update_fn(tradeDO_Ptr->UserID(), [&tradeDO_Ptr](cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr>& tradeMap)
	{
		tradeMap.map().insert(tradeDO_Ptr->TradeID, tradeDO_Ptr);
	});

	_tradeIdMap.insert(tradeDO_Ptr->TradeID, tradeDO_Ptr);
}

void UserTradeContext::AddTrade(const TradeRecordDO& tradeDO)
{
	AddTrade(std::make_shared<TradeRecordDO>(tradeDO));
}

void UserTradeContext::Clear(void)
{
	_userTradeMap.clear();
	_tradeIdMap.clear();
}

TradeRecordDO_Ptr UserTradeContext::RemoveTrade(uint64_t orderID)
{
	TradeRecordDO_Ptr ret;
	if (_tradeIdMap.find(orderID, ret))
	{
		_tradeIdMap.erase(orderID);

		cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr> tradeMap;
		if(_userTradeMap.find(ret->UserID(), tradeMap))
		{
			tradeMap.map().erase(orderID);
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

vector_ptr<TradeRecordDO> UserTradeContext::GetTradesByUser(const std::string & userID)
{
	auto ret = std::make_shared<std::vector<TradeRecordDO>>();
	cuckoohashmap_wrapper<uint64_t, TradeRecordDO_Ptr> tradeMap;
	if (_userTradeMap.find(userID, tradeMap))
	{
		auto lt = tradeMap.map().lock_table();
		for (auto& ut : lt)
		{
			ret->push_back(*ut.second);
		}
	}

	return ret;
}


TradeRecordDO_Ptr UserTradeContext::FindTrade(uint64_t orderID)
{
	TradeRecordDO_Ptr ret;
	_tradeIdMap.find(orderID, ret);

	return ret;
}