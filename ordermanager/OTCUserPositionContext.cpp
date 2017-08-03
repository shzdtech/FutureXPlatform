/***********************************************************************
 * Module:  OTCUserOrderContext.cpp
 * Author:  milk
 * Modified: 2016年1月26日 17:31:32
 * Purpose: Implementation of the class OTCUserOrderContext
 ***********************************************************************/

#include "OTCUserPositionContext.h"
#include "../pricingengine/PricingUtility.h"
#include "../utility/atomicutil.h"
#include "../utility/epsdouble.h"

UserPositionExDO_Ptr OTCUserPositionContext::UpsertPosition(const std::string & userid, const UserPositionExDO & positionDO)
{
	if (!_userPositionMap.contains(userid))
		_userPositionMap.insert(userid, std::move(OTCPortfolioPositionType(true, 2)));

	OTCPortfolioPositionType portfolioPosition;
	_userPositionMap.find(userid, portfolioPosition);

	if (!portfolioPosition.map()->contains(positionDO.PortfolioID()))
		portfolioPosition.map()->insert(positionDO.PortfolioID(), std::move(OTCPositionType(true, 16)));

	OTCPositionType contractPosition;
	portfolioPosition.map()->find(positionDO.PortfolioID(), contractPosition);

	auto newPosition_Ptr = std::make_shared<UserPositionExDO>(positionDO);
	newPosition_Ptr->TdPosition = 0;
	newPosition_Ptr->YdPosition = 0;
	newPosition_Ptr->OpenAmount = 0;
	newPosition_Ptr->OpenVolume = 0;
	newPosition_Ptr->CloseAmount = 0;
	newPosition_Ptr->CloseVolume = 0;

	contractPosition.map()->upsert(*newPosition_Ptr,
		[&newPosition_Ptr](UserPositionExDO_Ptr& position_ptr)
	{
		position_ptr->YdCost = newPosition_Ptr->YdCost;
		position_ptr->YdProfit = newPosition_Ptr->YdProfit;
		position_ptr->PreSettlementPrice = newPosition_Ptr->PreSettlementPrice;
		position_ptr->SettlementPrice = newPosition_Ptr->SettlementPrice;
		position_ptr->CloseProfit = newPosition_Ptr->CloseProfit;
		position_ptr->CashIn = newPosition_Ptr->CashIn;
		position_ptr->UseMargin = newPosition_Ptr->UseMargin;
		position_ptr->TdCost = newPosition_Ptr->TdCost;
		position_ptr->TdProfit = newPosition_Ptr->TdProfit;

		if (position_ptr->TradingDay == newPosition_Ptr->TradingDay &&
			position_ptr->YdInitPosition != newPosition_Ptr->YdInitPosition &&
			newPosition_Ptr->YdInitPosition > 0)
		{
			position_ptr->YdInitPosition = newPosition_Ptr->YdInitPosition;

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

		*newPosition_Ptr = *position_ptr;
	}, newPosition_Ptr);

	return newPosition_Ptr;
}

UserPositionExDO_Ptr OTCUserPositionContext::UpsertPosition(const std::string & userid, const TradeRecordDO_Ptr & tradeDO)
{
	if (!_userPositionMap.contains(userid))
		_userPositionMap.insert(userid, std::move(OTCPortfolioPositionType(true, 2)));

	OTCPortfolioPositionType portfolioPosition;
	_userPositionMap.find(userid, portfolioPosition);

	if (!portfolioPosition.map()->contains(tradeDO->PortfolioID()))
		portfolioPosition.map()->insert(tradeDO->PortfolioID(), std::move(OTCPositionType(true, 16)));

	OTCPositionType contractPosition;
	portfolioPosition.map()->find(tradeDO->PortfolioID(), contractPosition);

	// cost
	double cost = tradeDO->Price * tradeDO->Volume;

	// construct position
	UserPositionExDO_Ptr positionDO_Ptr(new UserPositionExDO(tradeDO->ExchangeID(), tradeDO->InstrumentID(), tradeDO->PortfolioID(), tradeDO->UserID()));
	positionDO_Ptr->TradingDay = tradeDO->TradingDay;

	if (tradeDO->Direction == DirectionType::SELL)
	{
		positionDO_Ptr->TdPosition = positionDO_Ptr->OpenVolume = -tradeDO->Volume;

		positionDO_Ptr->CloseVolume = tradeDO->Volume;
		positionDO_Ptr->CloseAmount = cost;
	}
	else
	{
		positionDO_Ptr->TdPosition = positionDO_Ptr->OpenVolume = tradeDO->Volume;
		positionDO_Ptr->OpenAmount = positionDO_Ptr->OpenCost = positionDO_Ptr->TdCost = cost;
	}

	contractPosition.map()->upsert(tradeDO->PortfolioID(), [&tradeDO, &positionDO_Ptr](UserPositionExDO_Ptr& position_ptr)
	{
		if (tradeDO->Direction == DirectionType::SELL)
		{
			position_ptr->CloseVolume += positionDO_Ptr->CloseVolume;
			position_ptr->CloseAmount += positionDO_Ptr->CloseAmount;

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
			position_ptr->TdPosition += positionDO_Ptr->OpenVolume;
			position_ptr->OpenVolume += positionDO_Ptr->OpenVolume;
			position_ptr->OpenAmount += positionDO_Ptr->OpenAmount;
			position_ptr->OpenCost += positionDO_Ptr->OpenAmount;
			position_ptr->TdCost += positionDO_Ptr->OpenAmount;
		}

		*positionDO_Ptr = *position_ptr;

	}, positionDO_Ptr);

	return positionDO_Ptr;
}

void OTCUserPositionContext::Clear(void)
{
	return _userPositionMap.clear();
}

OTCUserPositionType & OTCUserPositionContext::AllOTCPosition()
{
	return _userPositionMap;
}

OTCPortfolioPositionType OTCUserPositionContext::GetPortfolioPositionsByUser(const std::string & userID)
{
	OTCPortfolioPositionType ret;
	_userPositionMap.find(userID, ret);
	return ret;
}

OTCPositionType OTCUserPositionContext::GetPositionsByUser(const std::string & userID, const std::string & portfolio)
{
	OTCPositionType ret;

	OTCPortfolioPositionType portfolioPositition;
	_userPositionMap.find(userID, portfolioPositition);

	if (!portfolioPositition.empty())
	{
		portfolioPositition.map()->find(portfolio, ret);
	}

	return ret;
}

UserPositionExDO_Ptr OTCUserPositionContext::GetPosition(const std::string & userID, const std::string & portfolio, const ContractKey & contract)
{
	UserPositionExDO_Ptr ret;

	OTCPortfolioPositionType portfolioPosition;
	if (_userPositionMap.find(userID, portfolioPosition) && !portfolioPosition.empty())
	{
		OTCPositionType otcPosition;
		if (portfolioPosition.map()->find(portfolio, otcPosition) && !otcPosition.empty())
		{
			otcPosition.map()->find(contract, ret);
		}
	}
	return ret;
}

//int OTCUserPositionContext::UpdatePosition(
//	const StrategyContractDO& strategyDO,
//	DirectionType direction,
//	OrderOpenCloseType openClose,
//	int newTraded)
//{
//	int pos = -1;
//
//	if (newTraded > 0)
//	{
//		if (direction == DirectionType::SELL)
//			newTraded = -newTraded;
//
//		while (!_position.update_fn(strategyDO, [&](cuckoohashmap_wrapper<ContractKey, int, ContractKeyHash>& contractPos)
//		{
//			contractPos.map()->update_fn(strategyDO, [&](int& position)
//			{
//				pos = (position += newTraded);
//			});
//		}))
//		{
//			cuckoohashmap_wrapper<ContractKey, int, ContractKeyHash> contractPos;
//			contractPos.map()->insert(strategyDO, 0);
//			_position.insert(strategyDO, contractPos);
//		}
//	}
//
//	return pos;
//}

////////////////////////////////////////////////////////////////////////
// Name:       OTCHedgeHandler::GenSpreadOrders(const PortfolioKey& portfolioKey)
// Purpose:    Implementation of OTCHedgeHandler::GenSpreadOrders()
// Parameters:
// - userID
// - portfolio
// Return:     VectorDO_Ptr<OrderDO>
////////////////////////////////////////////////////////////////////////

bool OTCUserPositionContext::RemovePosition(const std::string & userID, const std::string & portfolio, const ContractKey & contract)
{
	bool ret = false;
	OTCPortfolioPositionType positionMap;
	if (_userPositionMap.find(userID, positionMap) && !positionMap.empty())
	{
		OTCPositionType position;
		if (positionMap.map()->find(portfolio, position) && !position.empty())
		{
			ret = position.map()->erase(contract);
		}
	}

	return ret;
}

ContractMap<double> OTCUserPositionContext::GenSpreadPoints(const PortfolioKey& portfolioKey)
{
	ContractMap<double> hedgeMap;
	//auto pStrategyMap = _pricingCtx->GetStrategyMap();

	double initPos = 0;
	double sumPos = 0;

	//_userPositionMap.update_fn(portfolioKey, [&](cuckoohashmap_wrapper<ContractKey, int, ContractKeyHash>& contractPos)
	//{
	//	static const double EPSILON = 1e-6;
	//	auto lt = contractPos.map()->lock_table();
	//	for (auto &it : lt)
	//	{
	//		int position = it.second;
	//		if (position != 0)
	//		{
	//			StrategyContractDO_Ptr strategy_ptr;
	//			if (pStrategyMap->find(it.first, strategy_ptr))
	//			{
	//				for (auto& bit : strategy_ptr->PricingContracts->PricingContracts)
	//				{
	//					double pos = bit.Weight*position;
	//					auto &mktpos = hedgeMap.getorfill(bit);
	//					atomicutil::round_add(mktpos, pos, EPSILON);
	//				}
	//			}
	//		}
	//	}
	//});

	return hedgeMap;
}


bool OTCUserPositionContext::GetRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
	const std::string& userID, const std::string& portfolio, UnderlyingRiskMap& risks)
{
	bool ret = false;
	auto positions = GetPositionsByUser(userID, portfolio);
	if (!positions.empty())
	{
		auto pStrategyMap = pricingCtx_Ptr->GetStrategyMap();
		auto pPricingData = pricingCtx_Ptr->GetPricingDataDOMap();
		for (auto it : positions.map()->lock_table())
		{
			auto userPosition_Ptr = it.second;
			if (userPosition_Ptr->Position() > 0)
			{
				IPricingDO_Ptr pricingDO;
				if (pPricingData->find(*userPosition_Ptr, pricingDO))
				{
					StrategyContractDO_Ptr strategy;
					if (pStrategyMap->find(*userPosition_Ptr, strategy) && strategy->PricingContracts)
					{
						for (auto pit : strategy->PricingContracts->PricingContracts)
						{
							int position = userPosition_Ptr->Position();
							if (auto pRiskDO = risks.getorfill(pit.Underlying).tryfind(*userPosition_Ptr))
							{
								pRiskDO->Delta += pit.Weight * pricingDO->Delta * position;
								pRiskDO->Gamma += pit.Weight * pricingDO->Gamma * position;
								pRiskDO->Theta += pit.Weight * pricingDO->Theta * position;
								pRiskDO->Vega += pit.Weight * pricingDO->Vega * position;
								pRiskDO->Rho += pit.Weight * pricingDO->Rho * position;
							}
							else
							{
								RiskDO riskDO(userPosition_Ptr->ExchangeID(), userPosition_Ptr->InstrumentID(), userPosition_Ptr->UserID());
								riskDO.Underlying = pit.Underlying;

								riskDO.Delta = pit.Weight * pricingDO->Delta * position;
								riskDO.Gamma = pit.Weight * pricingDO->Gamma * position;
								riskDO.Theta = pit.Weight * pricingDO->Theta * position;
								riskDO.Vega = pit.Weight * pricingDO->Vega * position;
								pRiskDO->Rho = pit.Weight * pricingDO->Rho * position;

								risks.getorfill(pit.Underlying).emplace(riskDO, riskDO);
							}
						}
					}
				}
			}
		}

		ret = true;
	}


	return ret;
}

bool OTCUserPositionContext::GetValuationRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
	const std::string & userID, const ValuationRiskDO& valuationRisk, UnderlyingRiskMap& risks)
{
	bool ret = false;
	auto positions = GetPositionsByUser(userID, valuationRisk.PortfolioID);
	if (!positions.empty())
	{
		auto pStrategyMap = pricingCtx_Ptr->GetStrategyMap();

		for (auto it : positions.map()->lock_table())
		{
			auto userPosition_Ptr = it.second;
			if (userPosition_Ptr->Position() > 0)
			{
				StrategyContractDO_Ptr strategy;
				if (pStrategyMap->find(*userPosition_Ptr, strategy) && strategy->PricingContracts)
				{
					for (auto pit : strategy->PricingContracts->PricingContracts)
					{
						if (auto pricingDO = PricingUtility::Pricing(&valuationRisk, *strategy, pricingCtx_Ptr))
						{
							int position = userPosition_Ptr->Position();
							if (auto pRiskDO = risks.getorfill(pit.Underlying).tryfind(*userPosition_Ptr))
							{
								pRiskDO->Delta += pit.Weight * pricingDO->Delta * position;
								pRiskDO->Gamma += pit.Weight * pricingDO->Gamma * position;
								pRiskDO->Theta += pit.Weight * pricingDO->Theta * position;
								pRiskDO->Vega += pit.Weight * pricingDO->Vega * position;
								pRiskDO->Rho += pit.Weight * pricingDO->Rho * position;
							}
							else
							{
								RiskDO riskDO(userPosition_Ptr->ExchangeID(), userPosition_Ptr->InstrumentID(), userPosition_Ptr->UserID());
								riskDO.Underlying = pit.Underlying;

								riskDO.Delta = pit.Weight * pricingDO->Delta * position;
								riskDO.Gamma = pit.Weight * pricingDO->Gamma * position;
								riskDO.Theta = pit.Weight * pricingDO->Theta * position;
								riskDO.Vega = pit.Weight * pricingDO->Vega * position;
								riskDO.Rho = pit.Weight * pricingDO->Rho * position;
								riskDO.Price = (pricingDO->Ask().Price + pricingDO->Bid().Price) / 2;

								risks.getorfill(pit.Underlying).emplace(riskDO, riskDO);
							}
						}
					}
				}
			}
		}

		ret = true;
	}


	return ret;
}
