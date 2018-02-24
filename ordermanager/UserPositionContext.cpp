/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "UserPositionContext.h"

UserPositionExDO_Ptr UserPositionContext::UpsertPosition(const std::string& userid, const UserPositionExDO& positionDO,
	bool updateYdPosition, bool closeYdFirst, bool forceSync)
{
	if (!_userPositionMap.contains(userid))
		_userPositionMap.insert(userid, std::move(ContractPosition(true, 2)));

	auto newPosition_Ptr = std::make_shared<UserPositionExDO>(positionDO);
	newPosition_Ptr->TdPosition = 0;
	newPosition_Ptr->YdPosition = 0;
	newPosition_Ptr->OpenAmount = 0;
	newPosition_Ptr->OpenVolume = 0;
	newPosition_Ptr->CloseAmount = 0;
	newPosition_Ptr->CloseVolume = 0;

	_userPositionMap.update_fn(userid, [&newPosition_Ptr, closeYdFirst, updateYdPosition, forceSync](ContractPosition& positionMap)
	{
		positionMap.map()->upsert(std::pair<std::string, int>(newPosition_Ptr->InstrumentID(), newPosition_Ptr->Direction),
			[&newPosition_Ptr, closeYdFirst, updateYdPosition, forceSync](UserPositionExDO_Ptr& position_ptr)
		{
			if (updateYdPosition)
			{
				position_ptr->YdCost = newPosition_Ptr->YdCost;
			}
			else
			{
				position_ptr->PreSettlementPrice = newPosition_Ptr->PreSettlementPrice;
				position_ptr->SettlementPrice = newPosition_Ptr->SettlementPrice;
				position_ptr->CloseProfit = newPosition_Ptr->CloseProfit;
				position_ptr->CashIn = newPosition_Ptr->CashIn;
				position_ptr->UseMargin = newPosition_Ptr->UseMargin;
				position_ptr->TdCost = newPosition_Ptr->AvgCost() * newPosition_Ptr->TdPosition;
			}

			if (forceSync)
			{
				position_ptr->OpenAmount = newPosition_Ptr->OpenAmount;
				position_ptr->OpenVolume = newPosition_Ptr->OpenVolume;
				position_ptr->CloseAmount = newPosition_Ptr->CloseAmount;
				position_ptr->CloseVolume = newPosition_Ptr->CloseVolume;
			}

			if (position_ptr->TradingDay == newPosition_Ptr->TradingDay &&
				position_ptr->YdInitPosition != newPosition_Ptr->YdInitPosition &&
				newPosition_Ptr->YdInitPosition >= 0)
			{
				position_ptr->YdInitPosition = newPosition_Ptr->YdInitPosition;

				if (closeYdFirst)
				{
					if (position_ptr->CloseVolume > position_ptr->YdInitPosition)
					{
						position_ptr->YdPosition = -position_ptr->YdInitPosition;
						position_ptr->TdPosition = position_ptr->OpenVolume - (position_ptr->CloseVolume - position_ptr->YdInitPosition);
					}
					else
					{
						position_ptr->YdPosition = -position_ptr->CloseVolume;
						position_ptr->TdPosition = position_ptr->OpenVolume;
					}
				}
			}

			*newPosition_Ptr = *position_ptr;
		}, newPosition_Ptr);
	});

	return newPosition_Ptr;
}

void UserPositionContext::Clear(void)
{
	_userPositionMap.clear();
}

cuckoohash_map<std::string, ContractPosition>& UserPositionContext::AllUserPosition()
{
	return _userPositionMap;
}

bool UserPositionContext::AllPosition(std::vector<UserPositionExDO_Ptr>& positions)
{
	for (auto it : _userPositionMap.lock_table())
	{
		for (auto uit : it.second.map()->lock_table())
		{
			positions.push_back(uit.second);
		}
	}

	return true;
}

PortfolioPositionPnL UserPositionContext::GetPortfolioPositionsPnLByUser(const std::string & userID)
{
	PortfolioPositionPnL ret(true, 1);
	ret.map()->insert("", GetPositionsPnLByUser(userID));
	return ret;
}

ContractPositionPnL UserPositionContext::GetPositionsPnLByUser(const std::string & userID, const std::string & portfolio)
{
	ContractPositionPnL positionPnLMap;
	_userPositionPnLMap.find(userID, positionPnLMap);
	return positionPnLMap;
}

PositionPnLDO_Ptr UserPositionContext::GetPositionPnL(const std::string & userID, const std::string & instumentID, const std::string & portfolio)
{
	return PositionPnLDO_Ptr();
}

PortfolioPosition UserPositionContext::GetPortfolioPositionsByUser(const std::string & userID)
{
	PortfolioPosition ret(true, 1);
	ret.map()->insert("", GetPositionsByUser(userID));
	return ret;
}

ContractPosition UserPositionContext::GetPositionsByUser(const std::string & userID, const std::string& portfolio)
{
	ContractPosition positionMap;
	_userPositionMap.find(userID, positionMap);

	return positionMap;
}


UserPositionExDO_Ptr UserPositionContext::GetPosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio)
{
	UserPositionExDO_Ptr ret;
	ContractPosition positionMap;
	if (_userPositionMap.find(userID, positionMap))
	{
		positionMap.map()->find(std::pair<std::string, int>(instumentID, direction), ret);
	}

	return ret;
}


bool UserPositionContext::RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio)
{
	bool ret = false;
	ContractPosition positionMap;
	if (_userPositionMap.find(userID, positionMap))
	{
		ret = positionMap.map()->erase(std::pair<std::string, int>(instumentID, direction));
	}

	return ret;
}

bool UserPositionContext::RemoveUserPosition(const std::string & userID)
{
	return _userPositionMap.erase(userID);
}

UserPositionExDO_Ptr UserPositionContext::UpsertPosition(const std::string& userid, const TradeRecordDO_Ptr& tradeDO,
	int multiplier, bool closeYdFirst)
{
	UserPositionExDO_Ptr positionDO_Ptr;
	if (userid == tradeDO->UserID() &&
		_positionTradeIDMap.insert(tradeDO->TradeID128()))
	{
		if (!_userPositionMap.contains(userid))
			_userPositionMap.insert(userid, std::move(ContractPosition(true, 2)));

		// cost
		double cost = tradeDO->Price * tradeDO->Volume;
		cost *= multiplier;

		// construct position
		positionDO_Ptr.reset(new UserPositionExDO(tradeDO->ExchangeID(), tradeDO->InstrumentID(), tradeDO->PortfolioID(), tradeDO->UserID()));
		positionDO_Ptr->Multiplier = multiplier;
		positionDO_Ptr->TradingDay = tradeDO->TradingDay;
		positionDO_Ptr->Direction =
			(tradeDO->Direction == DirectionType::SELL && tradeDO->OpenClose == OrderOpenCloseType::OPEN) ||
			(tradeDO->Direction != DirectionType::SELL && tradeDO->OpenClose != OrderOpenCloseType::OPEN) ?
			PositionDirectionType::PD_SHORT : PositionDirectionType::PD_LONG;
		positionDO_Ptr->HedgeFlag = tradeDO->HedgeFlag;

		if (tradeDO->OpenClose == OrderOpenCloseType::OPEN)
		{
			positionDO_Ptr->TdPosition = positionDO_Ptr->OpenVolume = tradeDO->Volume;
			positionDO_Ptr->OpenAmount = positionDO_Ptr->OpenCost = positionDO_Ptr->TdCost = cost;
		}
		else
		{
			positionDO_Ptr->CloseVolume = tradeDO->Volume;
			positionDO_Ptr->CloseAmount = cost;

			if (tradeDO->OpenClose == OrderOpenCloseType::CLOSETODAY)
			{
				positionDO_Ptr->TdPosition = -tradeDO->Volume;
			}
			else
			{
				positionDO_Ptr->YdPosition = -tradeDO->Volume;
			}
		}

		_userPositionMap.update_fn(userid, [&tradeDO, &positionDO_Ptr, multiplier, closeYdFirst](ContractPosition& positionMap)
		{
			positionMap.map()->upsert(std::pair<std::string, int>(tradeDO->InstrumentID(), positionDO_Ptr->Direction),
				[&tradeDO, &positionDO_Ptr, multiplier, closeYdFirst](UserPositionExDO_Ptr& position_ptr)
			{
				if (tradeDO->OpenClose == OrderOpenCloseType::OPEN)
				{
					position_ptr->TdPosition += positionDO_Ptr->OpenVolume;
					position_ptr->OpenVolume += positionDO_Ptr->OpenVolume;
					position_ptr->OpenAmount += positionDO_Ptr->OpenAmount;
					position_ptr->OpenCost += positionDO_Ptr->OpenAmount;
					position_ptr->TdCost += positionDO_Ptr->OpenAmount;
				}
				else
				{
					position_ptr->CloseVolume += positionDO_Ptr->CloseVolume;
					position_ptr->CloseAmount += positionDO_Ptr->CloseAmount;

					if (closeYdFirst)
					{
						if (position_ptr->CloseVolume > position_ptr->YdInitPosition)
						{
							position_ptr->YdPosition = -position_ptr->YdInitPosition;
							position_ptr->TdPosition = position_ptr->OpenVolume - (position_ptr->CloseVolume - position_ptr->YdInitPosition);
						}
						else
						{
							position_ptr->YdPosition = -position_ptr->CloseVolume;
							position_ptr->TdPosition = position_ptr->OpenVolume;
						}
					}
					else
					{
						if (tradeDO->OpenClose == OrderOpenCloseType::CLOSETODAY)
						{
							position_ptr->TdPosition -= positionDO_Ptr->CloseVolume;
						}
						else
						{
							position_ptr->YdPosition -= positionDO_Ptr->CloseVolume;
						}
					}

					double tdmean = position_ptr->OpenVolume ? position_ptr->OpenAmount / position_ptr->OpenVolume : 0;
					position_ptr->TdCost = tdmean * position_ptr->TdPosition;
					position_ptr->YdCost = position_ptr->PreSettlementPrice * position_ptr->LastPosition();
					position_ptr->YdCost *= multiplier;
				}

				positionDO_Ptr = position_ptr;

			}, positionDO_Ptr);
		});
	}

	return positionDO_Ptr;
}

int UserPositionContext::UpdatePnL(const std::string & userID, const MarketDataDO& mdDO)
{
	int ret = 0;
	if (auto contractPosition = GetPositionsByUser(userID))
	{
		for (auto pair : contractPosition.map()->lock_table())
		{
			if (mdDO.InstrumentID() == pair.second->InstrumentID())
			{
				pair.second->LastPrice = mdDO.LastPrice;
				pair.second->PreSettlementPrice = mdDO.PreSettlementPrice;
			}
		}
	}

	return ret;
}

bool UserPositionContext::FreezePosition(const OrderRequestDO& orderRequestDO, int& todayVol, int& ydVol)
{
	bool ret = true;
	todayVol = orderRequestDO.Volume;
	ydVol = 0;
	if (orderRequestDO.OpenClose != OrderOpenCloseType::OPEN)
	{
		ret = ret && _userPositionMap.update_fn(orderRequestDO.UserID(),
			[&orderRequestDO, &ret, &todayVol, &ydVol](ContractPosition& positionMap)
		{
			PositionDirectionType pd = orderRequestDO.Direction == DirectionType::SELL ? PositionDirectionType::PD_LONG : PositionDirectionType::PD_SHORT;
			ret = ret &&  positionMap.map()->update_fn(std::pair<std::string, int>(orderRequestDO.InstrumentID(), pd),
				[&orderRequestDO, &ret, &todayVol, &ydVol](UserPositionExDO_Ptr& position_ptr)
			{
				//if (position_ptr->Position() - position_ptr->FrozenVolume >= orderRequestDO.Volume)
				//{
				//	int tdRemain = position_ptr->TdPosition - position_ptr->FrozenVolume;
				//	if (tdRemain > 0)
				//	{
				//		int remain = tdRemain - orderRequestDO.Volume;
				//		if (remain < 0)
				//		{
				//			todayVol = tdRemain;
				//			ydVol = -remain;
				//		}
				//	}
				//	else
				//	{
				//		ydVol = orderRequestDO.Volume;
				//	}

				//	position_ptr->FrozenVolume += orderRequestDO.Volume;
				//}
				//else
				//{
				//	ret = false;
				//}
			});
		});
	}

	return ret;
}

bool UserPositionContext::GetRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
	const std::string & userID, const std::string & portfolio, UnderlyingRiskMap& risks)
{
	return false;
}

bool UserPositionContext::GetValuationRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
	const std::string & userID, const ValuationRiskDO& valuationRisk, UnderlyingRiskMap& risks)
{
	return false;
}

bool UserPositionContext::ContainsTrade(uint128 tradeID)
{
	return _positionTradeIDMap.contains(tradeID);
}
