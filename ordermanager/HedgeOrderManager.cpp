/***********************************************************************
 * Module:  HedgeOrderManager.cpp
 * Author:  milk
 * Modified: 2016年1月15日 23:39:13
 * Purpose: Implementation of the class HedgeOrderManager
 ***********************************************************************/

#include "HedgeOrderManager.h"
#include "../utility/atomicutil.h"
#include "../utility/epsdouble.h"
#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"

HedgeOrderManager::HedgeOrderManager(
	const IPricingDataContext_Ptr& pricingCtx, const IUserPositionContext_Ptr& exchangePositionCtx)
	: AutoOrderManager(pricingCtx, exchangePositionCtx), _updatingPortfolioLock(1024)
{

}

////////////////////////////////////////////////////////////////////////
// Name:       HedgeOrderManager::UpdateByStrategy(const StrategyContractDO& strategyDO)
// Purpose:    Implementation of HedgeOrderManager::UpdateByStrategy()
// Parameters:
// - strategyDO
// Return:     OrderDOVec_Ptr
////////////////////////////////////////////////////////////////////////

void HedgeOrderManager::TradeByStrategy(
	const StrategyContractDO& strategyDO, IOrderAPI* orderAPI)
{
	Hedge(strategyDO, orderAPI);
}

////////////////////////////////////////////////////////////////////////
// Name:       HedgeOrderManager::Hedge()
// Purpose:    Implementation of HedgeOrderManager::Hedge()
// Return:     int
////////////////////////////////////////////////////////////////////////

HedgeOrderManager::HedgeStatus HedgeOrderManager::Hedge(const PortfolioKey& portfolioKey, IOrderAPI* orderAPI)
{
	HedgeOrderManager::HedgeStatus ret = HedgingOff;

	PortfolioDO* pPortfolio = nullptr;
	if (auto pPortfolioMap = _pricingCtx->GetPortfolioMap())
	{
		if (auto pUserPortfolios = pPortfolioMap->tryfind(portfolioKey.UserID()))
		{
			pPortfolio = pUserPortfolios->tryfind(portfolioKey.PortfolioID());
		}
	}

	if (!pPortfolio || !pPortfolio->Hedging)
		return ret;

	_updatingPortfolioLock.upsert(*pPortfolio, [](bool& lock) { lock = true; }, true);

	_updatingPortfolioLock.update_fn(*pPortfolio, [this, &ret, pPortfolio, orderAPI](bool& lock)
	{
		ret = Waiting;

		UnderlyingRiskMap underlyingRiskMap;
		_exchangePositionCtx->GetRiskByPortfolio(_pricingCtx, pPortfolio->UserID(), pPortfolio->PortfolioID(), underlyingRiskMap);

		auto duration = std::chrono::steady_clock::now() - pPortfolio->LastHedge;

		if (std::chrono::duration_cast<std::chrono::seconds>(duration).count() > pPortfolio->HedgeDelay)
		{
			pPortfolio->HedingFlag = true;
		}
		else
		{
			pPortfolio->HedingFlag = false;
		}

		for (auto pair : underlyingRiskMap)
		{
			auto& riskDOMap = pair.second;

			double totalDelta = 0;

			for (auto& risk : riskDOMap)
			{
				totalDelta += risk.second.Delta;
			}

			if (std::abs(totalDelta) <= pPortfolio->Threshold)
			{
				pPortfolio->HedingFlag = false;
				continue;
			}

			pPortfolio->HedingFlag = true;

			auto& underlying = pair.first;

			auto it = pPortfolio->HedgeContracts.find(underlying);

			if (it == pPortfolio->HedgeContracts.end() || !it->second)
				continue;

			auto hedgeContract = it->second;

			double hedgeDelta = 1;
			IPricingDO_Ptr pricingDO;
			if (_pricingCtx->GetPricingDataDOMap()->find(*hedgeContract, pricingDO))
			{
				hedgeDelta = pricingDO->Delta;
			}

			if (std::abs(totalDelta) < std::abs(hedgeDelta))
			{
				continue;
			}

			OrderContractInnerMapType userOrderMap;

			if (!_userOrderCtx.UserOrderMap().find(pPortfolio->UserID(), userOrderMap))
			{
				_userOrderCtx.UserOrderMap().insert(pPortfolio->UserID(), 
					std::move(OrderContractInnerMapType(true, 16)));

				_userOrderCtx.UserOrderMap().find(pPortfolio->UserID(), userOrderMap);
			}

			OrderIDInnerMapType orders;
			if (!userOrderMap.map()->find(hedgeContract->InstrumentID(), orders))
			{
				userOrderMap.map()->insert(hedgeContract->InstrumentID(),
					std::move(OrderIDInnerMapType(true, 4)));

				userOrderMap.map()->find(hedgeContract->InstrumentID(), orders);
			}

			MarketDataDO mdo;
			_pricingCtx->GetMarketDataMap()->find(hedgeContract->InstrumentID(), mdo);
			if (mdo.Ask().Price < DBL_EPSILON)
				continue;

			std::vector<uint64_t> removeList;

			double pricingBid = mdo.Bid().Price;
			double pricingAsk = mdo.Ask().Price;

			double sellOpenVol = 0;
			double buyOpenVol = 0;

			double sellCloseVol = 0;
			double buyCloseVol = 0;

			for (auto& pair : orders.map()->lock_table())
			{
				auto& order = pair.second;

				if (order->PortfolioID() == pPortfolio->PortfolioID())
				{
					int vol = order->Volume;
					if (!order->Active)
					{
						vol = order->VolumeTraded;
						removeList.push_back(order->OrderID);
					}

					if (order->Direction == DirectionType::SELL)
					{
						if (order->OpenClose == OrderOpenCloseType::OPEN)
						{
							sellOpenVol += vol;
						}
						else
						{
							sellCloseVol += vol;
						}

						if (order->Active && (totalDelta < 0 || order->LimitPrice > pricingBid))
						{
							orderAPI->CancelOrder(*order);
						}
					}
					else
					{
						if (order->OpenClose == OrderOpenCloseType::OPEN)
						{
							buyOpenVol += vol;
						}
						else
						{
							buyCloseVol += vol;
						}

						if (order->Active && (totalDelta > 0 || order->LimitPrice < pricingAsk))
						{
							orderAPI->CancelOrder(*order);
						}
					}
				}
			}

			for (auto it : removeList)
			{
				_userOrderCtx.RemoveOrder(it);
			}

			int remainVol = 0;
			if (totalDelta > 0)
			{
				remainVol = (totalDelta / hedgeDelta) - (sellOpenVol + sellCloseVol);
			}
			else
			{
				remainVol = std::abs(totalDelta / hedgeDelta) - (buyOpenVol + buyCloseVol);
			}

			if (remainVol <= 0)
				continue;

			LOG_DEBUG << "Total Delta: " << totalDelta << ", RemainVol: " << remainVol;

			// Check max order volume
			if (auto pInstumentInfo = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).QueryInstrumentOrAddById(hedgeContract->InstrumentID()))
			{
				remainVol = std::min(pInstumentInfo->MaxLimitOrderVolume, remainVol);
			}

			if (pPortfolio->HedgeVolume > 0)
			{
				remainVol = std::min(pPortfolio->HedgeVolume, remainVol);
			}

			// Make new orders
			OrderRequestDO newOrder(0, hedgeContract->ExchangeID(), hedgeContract->InstrumentID(), pPortfolio->UserID(), pPortfolio->PortfolioID());
			newOrder.TIF = OrderTIFType::IOC;
			newOrder.VolCondition = OrderVolType::ANYVOLUME;

			bool isSHFE = newOrder.ExchangeID() == "SHFE";

			if (totalDelta > 0) // Long position
			{
				newOrder.Direction = DirectionType::SELL;
				newOrder.LimitPrice = pricingBid;

				auto longPos_Ptr = _exchangePositionCtx->GetPosition(pPortfolio->UserID(), hedgeContract->InstrumentID(),
					PositionDirectionType::PD_LONG, pPortfolio->PortfolioID());

				// Auto Close
				int availableYD = longPos_Ptr ? longPos_Ptr->LastPosition() - sellCloseVol : 0;
				if (isSHFE && availableYD > 0)
				{
					newOrder.OpenClose = OrderOpenCloseType::CLOSEYESTERDAY;
					newOrder.Volume = std::min(remainVol, availableYD);
					if (newOrder.Volume > 0 && CreateOrder(newOrder, orderAPI))
					{
						remainVol -= newOrder.Volume;
					}
				}
				else
				{
					newOrder.OpenClose = OrderOpenCloseType::CLOSE;
					availableYD = longPos_Ptr ? longPos_Ptr->Position() - sellCloseVol : 0;
					newOrder.Volume = std::min(remainVol, availableYD);
					if (newOrder.Volume > 0 && CreateOrder(newOrder, orderAPI))
					{
						remainVol -= newOrder.Volume;
					}
				}

				// Open remain
				if (remainVol > 0)
				{
					newOrder.OrderID = 0;
					newOrder.OpenClose = OrderOpenCloseType::OPEN;
					newOrder.Volume = remainVol;
					CreateOrder(newOrder, orderAPI);
				}
			}
			else // Short position
			{
				newOrder.Direction = DirectionType::BUY;
				newOrder.LimitPrice = pricingAsk;

				auto shortPos_Ptr = _exchangePositionCtx->GetPosition(pPortfolio->UserID(), hedgeContract->InstrumentID(),
					PositionDirectionType::PD_SHORT, pPortfolio->PortfolioID());

				// Auto Close
				int availableYD = shortPos_Ptr ? shortPos_Ptr->LastPosition() - buyCloseVol : 0;
				if (isSHFE && availableYD > 0)
				{
					newOrder.OpenClose = OrderOpenCloseType::CLOSEYESTERDAY;
					newOrder.Volume = std::min(remainVol, availableYD);
					if (newOrder.Volume > 0 &&
						CreateOrder(newOrder, orderAPI))
					{
						remainVol -= newOrder.Volume;
					}
				}
				else
				{
					newOrder.OpenClose = OrderOpenCloseType::CLOSE;
					availableYD = shortPos_Ptr ? shortPos_Ptr->Position() - buyCloseVol : 0;
					newOrder.Volume = std::min(remainVol, availableYD);
					if (newOrder.Volume > 0 &&
						CreateOrder(newOrder, orderAPI))
					{
						remainVol -= newOrder.Volume;
					}
				}

				// Open remain
				if (remainVol > 0)
				{
					newOrder.OrderID = 0;
					newOrder.OpenClose = OrderOpenCloseType::OPEN;
					newOrder.Volume = remainVol;
					CreateOrder(newOrder, orderAPI);
				}
			}
		}

		//if (pPortfolio->HedingFlag)
		//{
		//	ret = Hedgded;
		//	pPortfolio->HedingFlag = false;
		//}

		pPortfolio->LastHedge = std::chrono::steady_clock::now();

		lock = true;
	});

	return ret;
}

OrderDO_Ptr HedgeOrderManager::CancelOrder(OrderRequestDO& orderReq, IOrderAPI* orderAPI)
{
	OrderDO_Ptr ret;
	if (orderReq.OrderID != 0)
	{
		if (ret = FindOrder(orderReq.OrderID))
		{
			ret = orderAPI->CancelOrder(orderReq);
			// _userOrderCtx.RemoveOrder(orderReq.OrderID);
		}
	}
	else
	{
		if (auto pPortfolioMap = _pricingCtx->GetPortfolioMap())
		{
			if (auto pUserPortfolios = pPortfolioMap->tryfind(orderReq.UserID()))
			{
				if (auto pPortfolio = pUserPortfolios->tryfind(orderReq.PortfolioID()))
				{
					for (auto& pair : pPortfolio->HedgeContracts)
					{
						OrderContractInnerMapType userOrderMap;
						if (_userOrderCtx.UserOrderMap().find(orderReq.UserID(), userOrderMap))
						{
							OrderIDInnerMapType orders;
							if (userOrderMap.map()->find(orderReq.InstrumentID(), orders))
							{// std::vector<uint64_t> orderCancelList;
								for (auto& pair : orders.map()->lock_table())
								{
									if (pair.second->PortfolioID() == orderReq.PortfolioID())
										orderAPI->CancelOrder(*pair.second);
								}

								/*for (auto orderId : orderCancelList)
								_userOrderCtx.RemoveOrder(orderId);*/
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

int HedgeOrderManager::OnMarketOrderUpdated(OrderDO& orderInfo, IOrderAPI* orderAPI)
{
	int ret = 0;

	auto orderId = ParseOrderID(orderInfo.OrderID, orderInfo.SessionID, orderAPI);

	if (auto order_ptr = FindOrder(orderId))
	{
		switch (orderInfo.OrderStatus)
		{
		case OrderStatusType::ALL_TRADED:
		case OrderStatusType::PARTIAL_TRADING:
			ret = orderInfo.VolumeTraded - order_ptr->VolumeTraded;
			break;
		case OrderStatusType::CANCELED:
			ret = -1;
			break;
		default:
			ret = 0;
			break;
		}

		order_ptr->Active = orderInfo.Active;
		order_ptr->VolumeTraded = orderInfo.VolumeTraded;
		order_ptr->OrderStatus = orderInfo.OrderStatus;
	}

	return ret;
}