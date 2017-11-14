/***********************************************************************
 * Module:  UserPosTradeContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserPosTradeContext
 ***********************************************************************/

#include "UserTradeContext.h"

UserTradeContext::UserTradeContext()
	: _tradeIdMap(2048), _userTradeMap(1024)
{
}

bool UserTradeContext::InsertTrade(const TradeRecordDO_Ptr & tradeDO_Ptr)
{
	auto tradeID = tradeDO_Ptr->TradeID128();
	bool ret = _tradeIdMap.insert(tradeID, tradeDO_Ptr);
	if (ret)
	{
		if (!_userTradeMap.contains(tradeDO_Ptr->UserID()))
			_userTradeMap.insert(tradeDO_Ptr->UserID(), std::move(UserTradeMapType(true)));

		_userTradeMap.update_fn(tradeDO_Ptr->UserID(), [this, tradeID](UserTradeMapType& tradeMap)
		{
			if (auto instoreptr = FindTrade(tradeID))
			{
				tradeMap.map()->insert(tradeID, instoreptr);
			}
		});
	}

	return ret;
}

bool UserTradeContext::InsertTrade(const TradeRecordDO& tradeDO)
{
	return InsertTrade(std::make_shared<TradeRecordDO>(tradeDO));
}

void UserTradeContext::Clear(void)
{
	_userTradeMap.clear();
	_tradeIdMap.clear();
}

TradeRecordDO_Ptr UserTradeContext::RemoveTrade(uint128 tradeID)
{
	TradeRecordDO_Ptr ret;
	if (_tradeIdMap.find(tradeID, ret))
	{
		_tradeIdMap.erase(tradeID);

		UserTradeMapType tradeMap;
		if (_userTradeMap.find(ret->UserID(), tradeMap))
		{
			tradeMap.map()->erase(tradeID);
		}
	}
	return ret;
}

TradeIDMapType& UserTradeContext::GetAllTrade()
{
	return _tradeIdMap;
}

TradeMapType& UserTradeContext::UserTradeMap()
{
	return _userTradeMap;
}

UserTradeMapType UserTradeContext::GetTradesByUser(const std::string & userID)
{
	UserTradeMapType ret;
	_userTradeMap.find(userID, ret);

	return ret;
}

bool UserTradeContext::Contains(uint128 tradeID)
{
	return _tradeIdMap.contains(tradeID);
}



TradeRecordDO_Ptr UserTradeContext::FindTrade(uint128 tradeID)
{
	TradeRecordDO_Ptr ret;
	_tradeIdMap.find(tradeID, ret);

	return ret;
}