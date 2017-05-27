/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "PortfolioPositionContext.h"
#include "../bizutility/ContractCache.h"

PortfolioPositionContext::PortfolioPositionContext(const IPricingDataContext_Ptr & pricingCtx)
	: _pricingCtx(pricingCtx)
{
}

UserPositionExDO_Ptr PortfolioPositionContext::UpsertPosition(const std::string& userid, const UserPositionExDO& positionDO, bool updateYdPosition, bool closeYdFirst)
{
	if (!_userPositionMap.contains(userid))
		_userPositionMap.insert(userid, std::move(PortfolioPosition(true, 2)));

	PortfolioPosition portfolioPosition;
	_userPositionMap.find(userid, portfolioPosition);

	if (!portfolioPosition.map()->contains(positionDO.PortfolioID()))
		portfolioPosition.map()->insert(positionDO.PortfolioID(), std::move(ContractPosition(true, 16)));

	ContractPosition contractPosition;
	portfolioPosition.map()->find(positionDO.PortfolioID(), contractPosition);

	auto newPosition_Ptr = std::make_shared<UserPositionExDO>(positionDO);
	newPosition_Ptr->TdPosition = 0;
	newPosition_Ptr->YdPosition = 0;
	newPosition_Ptr->OpenAmount = 0;
	newPosition_Ptr->OpenVolume = 0;
	newPosition_Ptr->CloseAmount = 0;
	newPosition_Ptr->CloseVolume = 0;

	//if (newPosition_Ptr->PortfolioID().empty())
	//{
	//	StrategyContractDO_Ptr sto;
	//	if (_pricingCtx->GetStrategyMap()->find(positionDO, sto))
	//	{
	//		newPosition_Ptr->SetPortfolioID(sto->PortfolioID());
	//	}
	//}

	contractPosition.map()->upsert(std::pair<std::string, int>(newPosition_Ptr->InstrumentID(), newPosition_Ptr->Direction),
		[&newPosition_Ptr, closeYdFirst, updateYdPosition](UserPositionExDO_Ptr& position_ptr)
	{
		if (updateYdPosition)
		{
			position_ptr->YdCost = newPosition_Ptr->YdCost;
			position_ptr->YdProfit = newPosition_Ptr->YdProfit;
		}
		else
		{
			position_ptr->PreSettlementPrice = newPosition_Ptr->PreSettlementPrice;
			position_ptr->SettlementPrice = newPosition_Ptr->SettlementPrice;
			position_ptr->CloseProfit = newPosition_Ptr->CloseProfit;
			position_ptr->CashIn = newPosition_Ptr->CashIn;
			position_ptr->UseMargin = newPosition_Ptr->UseMargin;
			position_ptr->TdCost = newPosition_Ptr->TdCost;
			position_ptr->TdProfit = newPosition_Ptr->TdProfit;
		}

		if (position_ptr->TradingDay == newPosition_Ptr->TradingDay &&
			position_ptr->YdInitPosition != newPosition_Ptr->YdInitPosition &&
			newPosition_Ptr->YdInitPosition > 0)
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

	return newPosition_Ptr;
}

void PortfolioPositionContext::Clear(void)
{
	_userPositionMap.clear();
}

UserPortfolioPosition& PortfolioPositionContext::AllUserPortfolioPosition()
{
	return _userPositionMap;
}

PortfolioPosition PortfolioPositionContext::GetPortfolioPositionsByUser(const std::string & userID)
{
	PortfolioPosition portfolioPositition;
	_userPositionMap.find(userID, portfolioPositition);
	return portfolioPositition;
}

ContractPosition PortfolioPositionContext::GetPositionsByUser(const std::string & userID, const std::string& portfolio)
{
	ContractPosition ret;
	PortfolioPosition portfolioPositition;
	if (_userPositionMap.find(userID, portfolioPositition))
	{
		portfolioPositition.map()->find(portfolio, ret);
	}

	return ret;
}


UserPositionExDO_Ptr PortfolioPositionContext::GetPosition(const std::string & userID,
	const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio)
{
	UserPositionExDO_Ptr ret;

	PortfolioPosition portfolioPosition;
	if (_userPositionMap.find(userID, portfolioPosition) && !portfolioPosition.empty())
	{
		ContractPosition contractPosition;
		if (portfolioPosition.map()->find(portfolio, contractPosition) && !contractPosition.empty())
		{
			contractPosition.map()->find(std::pair<std::string, int>(instumentID, direction), ret);
		}
	}
	return ret;
}


bool PortfolioPositionContext::RemovePosition(const std::string & userID,
	const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio)
{
	bool ret = false;
	PortfolioPosition portfolioPosition;
	if (_userPositionMap.find(userID, portfolioPosition) && !portfolioPosition.empty())
	{
		ContractPosition contractPosition;
		if (portfolioPosition.map()->find(portfolio, contractPosition) && !contractPosition.empty())
		{
			ret = contractPosition.map()->erase(std::pair<std::string, int>(instumentID, direction));
		}
	}

	return ret;
}

bool PortfolioPositionContext::AllPosition(std::vector<UserPositionExDO_Ptr>& positions)
{
	for (auto it : _userPositionMap.lock_table())
	{
		if (!it.second.empty())
		{
			for (auto uit : it.second.map()->lock_table())
			{
				if (!uit.second.empty())
				{
					for (auto pit : uit.second.map()->lock_table())
					{
						positions.push_back(pit.second);
					}
				}
			}
		}
	}

	return true;
}

bool PortfolioPositionContext::GetRiskByPortfolio(const std::string & userID, const std::string & portfolio, UnderlyingRiskMap& risks)
{
	bool ret = false;
	auto positions = GetPositionsByUser(userID, portfolio);
	if (!positions.empty())
	{
		auto pStrategyMap = _pricingCtx->GetStrategyMap();
		auto pPricingData = _pricingCtx->GetPricingDataDOMap();
		for (auto it : positions.map()->lock_table())
		{
			auto& userPosition_Ptr = it.second;
			int position = userPosition_Ptr->Position();

			if (position > 0)
			{
				double delta = 0;
				double gamma = 0;
				double theta = 0;
				double vega = 0;
				double rho = 0;

				std::shared_ptr<ContractKey> baseContract_Ptr;

				StrategyContractDO_Ptr strategy;
				if (pStrategyMap->find(*userPosition_Ptr, strategy))
				{
					baseContract_Ptr = strategy->BaseContract;

					IPricingDO_Ptr pricingDO;
					if (pPricingData->find(*userPosition_Ptr, pricingDO))
					{
						delta = pricingDO->Delta;
						gamma = pricingDO->Gamma;
						theta = pricingDO->Theta;
						vega = pricingDO->Vega;
						rho = pricingDO->Rho;

						if (!baseContract_Ptr && strategy->PricingContracts)
						{
							for (auto pit : strategy->PricingContracts->PricingContracts)
							{
								if (auto pRiskDO = risks.getorfill(pit.Underlying).tryfind(pit))
								{
									if (it.first.second == PD_LONG)
									{
										pRiskDO->Delta += pit.Weight * delta * position;
										pRiskDO->Gamma += pit.Weight * gamma * position;
										pRiskDO->Theta += pit.Weight * theta * position;
										pRiskDO->Vega += pit.Weight * vega * position;
										pRiskDO->Rho += pit.Weight * rho * position;
									}
									else
									{
										pRiskDO->Delta -= pit.Weight * delta * position;
										pRiskDO->Gamma -= pit.Weight * gamma * position;
										pRiskDO->Theta -= pit.Weight * theta * position;
										pRiskDO->Vega -= pit.Weight * vega * position;
										pRiskDO->Rho -= pit.Weight * rho * position;
									}
								}
								else
								{
									RiskDO riskDO(pit.ExchangeID(), pit.InstrumentID(), userPosition_Ptr->UserID());
									riskDO.Underlying = pit.Underlying;

									if (it.first.second == PD_LONG)
									{
										riskDO.Delta = pit.Weight * delta * position;
										riskDO.Gamma = pit.Weight * gamma * position;
										riskDO.Theta = pit.Weight * theta * position;
										riskDO.Vega = pit.Weight * vega * position;
										riskDO.Rho = pit.Weight * rho * position;
									}
									else
									{
										riskDO.Delta = -pit.Weight * delta * position;
										riskDO.Gamma = -pit.Weight * gamma * position;
										riskDO.Theta = -pit.Weight * theta * position;
										riskDO.Vega = -pit.Weight * vega * position;
										riskDO.Rho = -pit.Weight * rho * position;
									}
									risks.getorfill(pit.Underlying).emplace(riskDO, riskDO);
								}
							}

							continue;
						}
					}
				}

				InstrumentDO* pInstument = nullptr;
				if (baseContract_Ptr)
				{
					pInstument = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(baseContract_Ptr->InstrumentID());
				}
				else
				{
					if (pInstument = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentById(userPosition_Ptr->InstrumentID()))
					{
						if (pInstument->ContractType == ContractType::CONTRACTTYPE_FUTURE)
						{
							delta = 1;
						}
					}
				}

				if (pInstument)
				{
					if (auto pRiskDO = risks.getorfill(pInstument->ProductID).tryfind(*userPosition_Ptr))
					{
						if (it.first.second == PD_LONG)
						{
							pRiskDO->Delta += delta * position;
							pRiskDO->Gamma += gamma * position;
							pRiskDO->Theta += theta * position;
							pRiskDO->Vega += vega * position;
							pRiskDO->Rho += rho * position;
						}
						else
						{
							pRiskDO->Delta -= delta * position;
							pRiskDO->Gamma -= gamma * position;
							pRiskDO->Theta -= theta * position;
							pRiskDO->Vega -=vega * position;
							pRiskDO->Rho -= rho * position;
						}
					}
					else
					{
						RiskDO riskDO(userPosition_Ptr->ExchangeID(), userPosition_Ptr->InstrumentID(), userPosition_Ptr->UserID());
						riskDO.Underlying = pInstument->ProductID;

						if (it.first.second == PD_LONG)
						{
							riskDO.Delta = delta * position;
							riskDO.Gamma = gamma * position;
							riskDO.Theta = theta * position;
							riskDO.Vega = vega * position;
							riskDO.Rho = rho * position;
						}
						else
						{
							riskDO.Delta = -delta * position;
							riskDO.Gamma = -gamma * position;
							riskDO.Theta = -theta * position;
							riskDO.Vega = -vega * position;
							riskDO.Rho = -rho * position;
						}

						risks.getorfill(pInstument->ProductID).emplace(riskDO, riskDO);
					}
				}
			}
		}

		ret = true;
	}


	return ret;
}

UserPositionExDO_Ptr PortfolioPositionContext::UpsertPosition(const std::string& userid,
	const TradeRecordDO_Ptr& tradeDO, int multiplier, bool closeYdFirst)
{
	if (!_userPositionMap.contains(userid))
		_userPositionMap.insert(userid, std::move(PortfolioPosition(true, 2)));

	PortfolioPosition portfolioPosition;
	_userPositionMap.find(userid, portfolioPosition);

	if (!portfolioPosition.map()->contains(tradeDO->PortfolioID()))
		portfolioPosition.map()->insert(tradeDO->PortfolioID(), std::move(ContractPosition(true, 16)));

	ContractPosition contractPosition;
	portfolioPosition.map()->find(tradeDO->PortfolioID(), contractPosition);

	// cost
	double cost = tradeDO->Price * tradeDO->Volume;
	cost *= multiplier;

	// construct position
	UserPositionExDO_Ptr positionDO_Ptr(new UserPositionExDO(tradeDO->ExchangeID(), tradeDO->InstrumentID(), tradeDO->PortfolioID(), tradeDO->UserID()));

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

	contractPosition.map()->upsert(std::pair<std::string, int>(tradeDO->InstrumentID(), positionDO_Ptr->Direction),
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

		*positionDO_Ptr = *position_ptr;

	}, positionDO_Ptr);

	return positionDO_Ptr;
}