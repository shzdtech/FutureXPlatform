/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年11月22日 23:43:26
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "PortfolioPositionContext.h"
#include "../pricingengine/PricingUtility.h"
#include "../pricingengine/ModelAlgorithmManager.h"
#include "../pricingengine/OptionParams.h"
#include "../dataobject/TradingDeskOptionParams.h"
#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"
#include "../databaseop/ContractDAO.h"
#include "../litelogger/LiteLogger.h"

UserPositionExDO_Ptr PortfolioPositionContext::UpsertPosition(const std::string& userid, const UserPositionExDO& positionDO,
	bool updateYdPosition, bool closeYdFirst, bool forceSync)
{
	PortfolioPosition portfolioPosition;
	if (!_userPositionMap.find(userid, portfolioPosition))
	{
		_userPositionMap.insert(userid, std::move(PortfolioPosition(true, 2)));
		_userPositionMap.find(userid, portfolioPosition);
	}

	ContractPosition contractPosition;
	if (!portfolioPosition.map()->find(positionDO.PortfolioID(), contractPosition))
	{
		portfolioPosition.map()->insert(positionDO.PortfolioID(), std::move(ContractPosition(true, 16)));
		portfolioPosition.map()->find(positionDO.PortfolioID(), contractPosition);
	}

	PortfolioPositionPnL portfolioPositionPnL;
	if (!_userPositionPnLMap.find(userid, portfolioPositionPnL))
	{
		_userPositionPnLMap.insert(userid, std::move(PortfolioPositionPnL(true, 2)));
		_userPositionPnLMap.find(userid, portfolioPositionPnL);
	}

	ContractPositionPnL contractPositionPnL;
	if (!portfolioPositionPnL.map()->find(positionDO.PortfolioID(), contractPositionPnL))
	{
		portfolioPositionPnL.map()->insert(positionDO.PortfolioID(), std::move(ContractPositionPnL(true, 16)));
		portfolioPositionPnL.map()->find(positionDO.PortfolioID(), contractPositionPnL);
	}

	auto newPosition_Ptr = std::make_shared<UserPositionExDO>(positionDO);
	newPosition_Ptr->TdPosition = 0;
	newPosition_Ptr->YdPosition = 0;
	newPosition_Ptr->OpenAmount = 0;
	newPosition_Ptr->OpenVolume = 0;
	newPosition_Ptr->CloseAmount = 0;
	newPosition_Ptr->CloseVolume = 0;

	contractPosition.map()->upsert(std::pair<std::string, int>(newPosition_Ptr->InstrumentID(), newPosition_Ptr->Direction),
		[&positionDO, &newPosition_Ptr, closeYdFirst, updateYdPosition, forceSync](UserPositionExDO_Ptr& position_ptr)
	{
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

		if (updateYdPosition)
		{
			double ydMeanCost = 0;
			if (positionDO.YdPosition > 0)
				ydMeanCost = positionDO.YdCost / positionDO.LastPosition();

			position_ptr->YdCost = ydMeanCost * position_ptr->LastPosition();
		}
		else
		{
			position_ptr->PreSettlementPrice = newPosition_Ptr->PreSettlementPrice;
			position_ptr->SettlementPrice = newPosition_Ptr->SettlementPrice;
			position_ptr->CloseProfit = newPosition_Ptr->CloseProfit;
			position_ptr->CashIn = newPosition_Ptr->CashIn;
			position_ptr->UseMargin = newPosition_Ptr->UseMargin;
			position_ptr->TdCost = position_ptr->AvgCost() * position_ptr->TdPosition;
		}

		if (forceSync)
		{
			position_ptr->OpenAmount = positionDO.OpenAmount;
			position_ptr->OpenVolume = positionDO.OpenVolume;
			position_ptr->CloseAmount = positionDO.CloseAmount;
			position_ptr->CloseVolume = positionDO.CloseVolume;
		}

		*newPosition_Ptr = *position_ptr;
	}, newPosition_Ptr);

	PositionPnLDO_Ptr positionPnLDO_Ptr =
		std::make_shared<PositionPnLDO>(positionDO.ExchangeID(), positionDO.InstrumentID(), positionDO.PortfolioID(), positionDO.UserID());
	positionPnLDO_Ptr->Multiplier = positionDO.Multiplier;
	positionPnLDO_Ptr->PreSettlementPrice = positionDO.PreSettlementPrice;
	positionPnLDO_Ptr->LastPrice = positionDO.LastPrice;

	if (positionDO.Direction == PositionDirectionType::PD_LONG)
		positionPnLDO_Ptr->YdBuyVolume = positionDO.YdInitPosition;

	if (positionDO.Direction == PositionDirectionType::PD_SHORT)
		positionPnLDO_Ptr->YdSellVolume = positionDO.YdInitPosition;

	contractPositionPnL.map()->upsert(positionDO.InstrumentID(),
		[&positionDO](PositionPnLDO_Ptr& positionPnL_ptr)
	{
		positionPnL_ptr->PreSettlementPrice = positionDO.PreSettlementPrice;
		positionPnL_ptr->LastPrice = positionDO.LastPrice;

		if (positionDO.Direction == PositionDirectionType::PD_LONG)
			positionPnL_ptr->YdBuyVolume = positionDO.YdInitPosition;
		else if (positionDO.Direction == PositionDirectionType::PD_SHORT)
			positionPnL_ptr->YdSellVolume = positionDO.YdInitPosition;

	}, positionPnLDO_Ptr);

	return newPosition_Ptr;
}

void PortfolioPositionContext::Clear(void)
{
	_userPositionMap.clear();
}

PortfolioPositionPnL PortfolioPositionContext::GetPortfolioPositionsPnLByUser(const std::string & userID)
{
	PortfolioPositionPnL portfolioPosititionPnL;
	_userPositionPnLMap.find(userID, portfolioPosititionPnL);
	return portfolioPosititionPnL;
}

ContractPositionPnL PortfolioPositionContext::GetPositionsPnLByUser(const std::string & userID, const std::string & portfolio)
{
	ContractPositionPnL ret;
	PortfolioPositionPnL portfolioPosititionPnL;
	if (_userPositionPnLMap.find(userID, portfolioPosititionPnL))
	{
		portfolioPosititionPnL.map()->find(portfolio, ret);
	}

	return ret;
}

PositionPnLDO_Ptr PortfolioPositionContext::GetPositionPnL(const std::string & userID, const std::string & instumentID, const std::string & portfolio)
{
	PositionPnLDO_Ptr ret;

	PortfolioPositionPnL portfolioPositionPnL;
	if (_userPositionPnLMap.find(userID, portfolioPositionPnL) && !portfolioPositionPnL.empty())
	{
		ContractPositionPnL contractPositionPnL;
		if (portfolioPositionPnL.map()->find(portfolio, contractPositionPnL) && !contractPositionPnL.empty())
		{
			contractPositionPnL.map()->find(instumentID, ret);
		}
	}
	return ret;
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

bool PortfolioPositionContext::RemoveUserPosition(const std::string & userID)
{
	return _userPositionMap.erase(userID);
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

bool PortfolioPositionContext::GetRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
	const std::string & userID, const std::string & portfolio, UnderlyingRiskMap& risks)
{
	bool ret = false;
	auto positions = GetPositionsByUser(userID, portfolio);
	if (!positions.empty())
	{
		auto pStrategyMap = pricingCtx_Ptr->GetStrategyMap();
		auto pPricingData = pricingCtx_Ptr->GetPricingDataDOMap();
		for (auto it : positions.map()->lock_table())
		{
			auto& userPosition_Ptr = it.second;
			int position = userPosition_Ptr->Position();

			if (position > 0)
			{
				LOG_DEBUG << userPosition_Ptr->InstrumentID() << ":" << userPosition_Ptr->Direction << ": " << position;

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
					else
					{
						LOG_WARN << "Cannot find pricing object: " << userPosition_Ptr->InstrumentID();
					}
				}

				auto& contractCache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);
				InstrumentDO* pInstument = nullptr;
				if (baseContract_Ptr)
				{
					pInstument = contractCache.QueryInstrumentOrAddById(baseContract_Ptr->InstrumentID());
				}
				else
				{
					pInstument = contractCache.QueryInstrumentOrAddById(userPosition_Ptr->InstrumentID());

					if (pInstument && pInstument->ContractType == ContractType::CONTRACTTYPE_FUTURE)
					{
						delta = 1;
					}
					else
					{
						continue;
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
							pRiskDO->Vega -= vega * position;
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

bool PortfolioPositionContext::GetValuationRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
	const std::string & userID, const ValuationRiskDO& valuationRisk, UnderlyingRiskMap& risks)
{
	bool ret = false;
	auto positions = GetPositionsByUser(userID, valuationRisk.PortfolioID);
	if (!positions.empty())
	{
		auto pStrategyMap = pricingCtx_Ptr->GetStrategyMap();

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
				double price = 0;

				std::shared_ptr<ContractKey> baseContract_Ptr;

				StrategyContractDO_Ptr strategy;
				if (pStrategyMap->find(*userPosition_Ptr, strategy) && strategy->PricingModel)
				{
					baseContract_Ptr = strategy->BaseContract;
					if (baseContract_Ptr)
					{
						IPricingDO_Ptr pricingDO;

						if (strategy->VolModel)
						{
							if (auto volmodel_ptr = ModelAlgorithmManager::Instance()->FindModel(strategy->VolModel->Model))
							{
								auto it = valuationRisk.ValuationPrice.find(baseContract_Ptr->InstrumentID());
								if (it == valuationRisk.ValuationPrice.end())
									continue;

								if (auto result = volmodel_ptr->Compute(&it->second.Price, *strategy, pricingCtx_Ptr, nullptr))
								{
									auto wresult = std::static_pointer_cast<WingsModelReturnDO>(result);
									OptionParams param;
									param.midVolatility = wresult->Volatility;
									param.bidVolatility = wresult->TBid().Volatility;
									param.askVolatility = wresult->TAsk().Volatility;

									if (auto pParams = (OptionParams*)strategy->PricingModel->ParsedParams.get())
									{
										param.dividend = pParams->dividend;
										param.riskFreeRate = pParams->riskFreeRate + valuationRisk.Interest;
									}

									param_vector params{ &param };
									pricingDO = PricingUtility::Pricing(&valuationRisk, *strategy, pricingCtx_Ptr, &params);
								}
							}
						}
						else
						{
							pricingDO = PricingUtility::Pricing(&valuationRisk, *strategy, pricingCtx_Ptr);
						}

						if (pricingDO)
						{
							delta = pricingDO->Delta;
							gamma = pricingDO->Gamma;
							theta = pricingDO->Theta;
							vega = pricingDO->Vega;
							rho = pricingDO->Rho;
							price = (pricingDO->Ask().Price + pricingDO->Bid().Price) / 2;
						}
					}
					else if (strategy->PricingContracts)
					{
						for (auto pit : strategy->PricingContracts->PricingContracts)
						{
							if (auto pricingDO = PricingUtility::Pricing(&valuationRisk, *strategy, pricingCtx_Ptr))
							{
								delta = pricingDO->Delta;
								gamma = pricingDO->Gamma;
								theta = pricingDO->Theta;
								vega = pricingDO->Vega;
								rho = pricingDO->Rho;
								price = (pricingDO->Ask().Price + pricingDO->Bid().Price) / 2;
							}

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

								riskDO.Price = price;

								risks.getorfill(pit.Underlying).emplace(riskDO, riskDO);
							}
						}

						continue;
					}
				}

				InstrumentDO* pInstument = nullptr;
				if (baseContract_Ptr)
				{
					pInstument = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(baseContract_Ptr->InstrumentID());
				}
				else
				{
					if (pInstument = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(userPosition_Ptr->InstrumentID()))
					{
						if (pInstument->ContractType == ContractType::CONTRACTTYPE_FUTURE)
						{
							delta = 1;

							auto it = valuationRisk.ValuationPrice.find(userPosition_Ptr->InstrumentID());
							if (it != valuationRisk.ValuationPrice.end())
								price = it->second.Price;
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
							pRiskDO->Vega -= vega * position;
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

						riskDO.Price = price;

						risks.getorfill(pInstument->ProductID).emplace(riskDO, riskDO);
					}
				}
			}
		}

		ret = true;
	}

	return ret;
}

bool PortfolioPositionContext::ContainsTrade(uint128 tradeID)
{
	return _positionTradeIDMap.contains(tradeID);
}


UserPositionExDO_Ptr PortfolioPositionContext::UpsertPosition(const std::string& userid,
	const TradeRecordDO_Ptr& tradeDO, int multiplier, bool closeYdFirst)
{
	UserPositionExDO_Ptr positionDO_Ptr;
	if (userid == tradeDO->UserID() && _positionTradeIDMap.insert(tradeDO->TradeID128()))
	{
		PortfolioPosition portfolioPosition;
		if (!_userPositionMap.find(userid, portfolioPosition))
		{
			_userPositionMap.insert(userid, std::move(PortfolioPosition(true, 2)));
			_userPositionMap.find(userid, portfolioPosition);
		}

		ContractPosition contractPosition;
		if (!portfolioPosition.map()->find(tradeDO->PortfolioID(), contractPosition))
		{
			portfolioPosition.map()->insert(tradeDO->PortfolioID(), std::move(ContractPosition(true, 16)));
			portfolioPosition.map()->find(tradeDO->PortfolioID(), contractPosition);
		}

		PortfolioPositionPnL portfolioPositionPnL;
		if (!_userPositionPnLMap.find(userid, portfolioPositionPnL))
		{
			_userPositionPnLMap.insert(userid, std::move(PortfolioPositionPnL(true, 2)));
			_userPositionPnLMap.find(userid, portfolioPositionPnL);
		}

		ContractPositionPnL contractPositionPnL;
		if (!portfolioPositionPnL.map()->find(tradeDO->PortfolioID(), contractPositionPnL))
		{
			portfolioPositionPnL.map()->insert(tradeDO->PortfolioID(), std::move(ContractPositionPnL(true, 16)));
			portfolioPositionPnL.map()->find(tradeDO->PortfolioID(), contractPositionPnL);
		}

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

			positionDO_Ptr = position_ptr;

		}, positionDO_Ptr);

		PositionPnLDO_Ptr positionPnLDO_Ptr =
			std::make_shared<PositionPnLDO>(tradeDO->ExchangeID(), tradeDO->InstrumentID(), tradeDO->PortfolioID(), tradeDO->UserID());
		positionPnLDO_Ptr->Multiplier = multiplier;
		positionPnLDO_Ptr->LastPrice = tradeDO->Price;
		if (tradeDO->Direction == DirectionType::SELL)
		{
			positionPnLDO_Ptr->TdSellVolume = tradeDO->Volume;
			positionPnLDO_Ptr->TdSellAmount = cost;
		}
		else
		{
			positionPnLDO_Ptr->TdBuyVolume = tradeDO->Volume;
			positionPnLDO_Ptr->TdBuyAmount = cost;
		}

		contractPositionPnL.map()->upsert(tradeDO->InstrumentID(),
			[&tradeDO, &positionPnLDO_Ptr, multiplier](PositionPnLDO_Ptr& positionPnL_ptr)
		{
			if (tradeDO->Direction == DirectionType::SELL)
			{
				positionPnL_ptr->TdSellVolume += positionPnLDO_Ptr->TdSellVolume;
				positionPnL_ptr->TdSellAmount += positionPnLDO_Ptr->TdSellAmount;
			}
			else
			{
				positionPnL_ptr->TdBuyVolume += positionPnLDO_Ptr->TdBuyVolume;
				positionPnL_ptr->TdBuyAmount += positionPnLDO_Ptr->TdBuyAmount;
			}
		}, positionPnLDO_Ptr);
	}

	return positionDO_Ptr;
}


int PortfolioPositionContext::UpdatePnL(const std::string & userID, const MarketDataDO& mdDO)
{
	int ret = 0;
	if (auto portfolioPositionPnL = GetPortfolioPositionsPnLByUser(userID))
	{
		for (auto pair : portfolioPositionPnL.map()->lock_table())
		{
			if (!pair.second.empty())
			{
				for (auto pPair : pair.second.map()->lock_table())
				{
					if (mdDO.InstrumentID() == pPair.second->InstrumentID())
					{
						pPair.second->LastPrice = mdDO.LastPrice;
						pPair.second->PreSettlementPrice = mdDO.PreSettlementPrice;
					}
				}
			}
		}
	}

	return ret;
}
