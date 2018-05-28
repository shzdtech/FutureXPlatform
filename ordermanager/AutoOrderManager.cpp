/***********************************************************************
* Module:  AutoOrderManager.cpp
* Author:  milk
* Modified: 2015年10月22日 23:49:43
* Purpose: Implementation of the class AutoOrderManager
***********************************************************************/

#include <thread>
#include <algorithm>
#include "../utility/epsdouble.h"
#include "AutoOrderManager.h"
#include "OrderSeqGen.h"
#include "OrderPortfolioCache.h"
#include "../databaseop/OrderDAO.h"
#include "../pricingengine/PricingUtility.h"


AutoOrderManager::AutoOrderManager(
	const IPricingDataContext_Ptr& pricingCtx,
	const IUserPositionContext_Ptr& exchangePositionCtx)
	: OrderManager(pricingCtx),
	_updatinglock(2048),
	_exchangePositionCtx(exchangePositionCtx)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       AutoOrderManager::CreateOrder(const OrderDO& orderReq)
// Purpose:    Implementation of AutoOrderManager::CreateOrder()
// Parameters:
// - orderReq
// Return:     OrderDO_Ptr
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr AutoOrderManager::CreateOrder(OrderRequestDO& orderReq, IOrderAPI* orderAPI)
{
	OrderDO_Ptr ret;

	if (orderReq.OrderID == 0 || !FindOrder(ParseOrderID(orderReq.OrderID, 0, orderAPI)))
	{
		orderReq.SessionID = orderAPI->GetSessionId();
		orderReq.OrderID = OrderSeqGen::GenOrderID(orderAPI->GetSessionId());
		_userOrderCtx.UpsertOrder(orderReq.OrderID, orderReq);
		ret = orderAPI->CreateOrder(orderReq);
		if (!ret)
		{
			_userOrderCtx.RemoveOrder(orderReq.OrderID);
		}
	}

	return ret;
}


////////////////////////////////////////////////////////////////////////
// Name:       AutoOrderManager::UpdateByStrategy(const StrategyContractDO& strategyDO)
// Purpose:    Implementation of AutoOrderManager::UpdateStatus()
// Parameters:
// - strategyDO
// Return:     int
////////////////////////////////////////////////////////////////////////

void AutoOrderManager::TradeByStrategy(const StrategyContractDO& strategyDO, IOrderAPI* orderAPI)
{
	if (!strategyDO.AutoOrderEnabled || strategyDO.IsOTC())
		return;

	_updatinglock.upsert(strategyDO, [](bool& lock) { lock = true; }, true);

	_updatinglock.update_fn(strategyDO, [this, &strategyDO, orderAPI](bool& lock)
	{
		OrderContractInnerMapType userOrderMap;
		if (!_userOrderCtx.UserOrderMap().find(strategyDO.UserID(), userOrderMap))
		{
			_userOrderCtx.UserOrderMap().insert(strategyDO.UserID(),
				std::move(OrderContractInnerMapType(true, 16)));

			_userOrderCtx.UserOrderMap().find(strategyDO.UserID(), userOrderMap);
		}

		OrderIDInnerMapType orders;
		if (!userOrderMap.map()->find(strategyDO.InstrumentID(), orders))
		{
			userOrderMap.map()->insert(strategyDO.InstrumentID(),
				std::move(OrderIDInnerMapType(true, std::max(strategyDO.Depth * 2, 2))));

			userOrderMap.map()->find(strategyDO.InstrumentID(), orders);
		}

		IPricingDO_Ptr pricingDO_ptr;
		if (_pricingCtx->GetPricingDataDOMap()->find(strategyDO, pricingDO_ptr))
		{
			MarketDataDO mdo;
			_pricingCtx->GetMarketDataMap()->find(strategyDO.InstrumentID(), mdo);
			if (mdo.Ask().Price < DBL_EPSILON)
			{
				mdo.LowerLimitPrice = 0;
				mdo.UpperLimitPrice = 1e32;
				mdo.Bid().Price = 1e32;
				mdo.Ask().Price = 0;
			}

			bool closeMode = strategyDO.AutoOrderSettings.CloseMode;
			bool fakMode = strategyDO.AutoOrderSettings.TIF == OrderTIFType::IOC;
			bool notCross = strategyDO.NotCross;

			double tickSize = strategyDO.EffectiveTickSize();
			bool askEnable = strategyDO.AskEnabled && strategyDO.AutoOrderSettings.AskCounter < strategyDO.AutoOrderSettings.MaxAutoTrade;
			bool bidEnable = strategyDO.BidEnabled && strategyDO.AutoOrderSettings.BidCounter < strategyDO.AutoOrderSettings.MaxAutoTrade;

			autofillmap<epsdouble, int> bidPriceOrders;
			autofillmap<epsdouble, int> askPriceOrders;

			if (fakMode)
			{
				if (pricingDO_ptr->Bid().Price >= mdo.Ask().Price)
				{
					bidPriceOrders.emplace(pricingDO_ptr->Bid().Price, 0);
				}
				else if (pricingDO_ptr->Ask().Price <= mdo.Bid().Price)
				{
					askPriceOrders.emplace(pricingDO_ptr->Ask().Price, 0);
				}
				else
				{
					return;
				}
			}
			else
			{
				int depth = closeMode ? 1 : strategyDO.Depth;

				double pricingBidMax = notCross ? std::min(pricingDO_ptr->Bid().Price, mdo.Bid().Price) : pricingDO_ptr->Bid().Price;
				pricingBidMax = std::floor(pricingBidMax / tickSize) * tickSize;
				double pricingBidMin = pricingBidMax - (depth - 1)*tickSize;

				double pricingAskMin = notCross ? std::max(pricingDO_ptr->Ask().Price, mdo.Ask().Price) : pricingDO_ptr->Ask().Price;
				pricingAskMin = std::ceil(pricingAskMin / tickSize) * tickSize;
				double pricingAskMax = pricingAskMin + (depth - 1)*tickSize;

				epsdouble askPrice(pricingAskMin);
				epsdouble bidPrice(pricingBidMax);
				for (int i = 0; i < depth; i++)
				{
					bidPriceOrders.emplace(bidPrice, 0);
					askPriceOrders.emplace(askPrice, 0);
					askPrice += tickSize;
					bidPrice -= tickSize;
				}
			}

			std::vector<OrderDO_Ptr> cancelList;
			int closeVol = 0;
			for (auto& pair : orders.map()->lock_table())
			{
				auto& order = pair.second;
				bool closeType = order->OpenClose != OrderOpenCloseType::OPEN;
				if (closeType)
					closeVol += order->Volume;

				if (order->Direction == DirectionType::SELL)
				{
					if (auto pTotalVol = askPriceOrders.tryfind(order->LimitPrice))
					{
						if (askEnable && closeMode == closeType)
						{
							if (order->Active)
							{
								*pTotalVol += order->Volume;
								if (*pTotalVol > strategyDO.AutoOrderSettings.AskQV)
									orderAPI->CancelOrder(*order);
							}
						}
						else if (order->Active)
						{
							cancelList.push_back(order);
						}
					}
					else if (order->Active)
					{
						cancelList.push_back(order);
					}
				}
				else
				{
					if (auto pTotalVol = bidPriceOrders.tryfind(order->LimitPrice))
					{
						if (bidEnable && closeMode == closeType)
						{
							if (order->Active)
							{
								*pTotalVol += order->Volume;
								if (*pTotalVol > strategyDO.AutoOrderSettings.BidQV)
									orderAPI->CancelOrder(*order);
							}
						}
						else if (order->Active)
						{
							cancelList.push_back(order);
						}
					}
					else if (order->Active)
					{
						cancelList.push_back(order);
					}
				}
			}

			for (auto order_ptr : cancelList)
			{
				orderAPI->CancelOrder(*order_ptr);
			}

			if (cancelList.empty() && (askEnable || bidEnable))
			{
				// Make new orders
				OrderRequestDO newOrder(strategyDO, strategyDO.PortfolioID());
				newOrder.TradingType = OrderTradingType::TRADINGTYPE_AUTO;

				UserPositionExDO_Ptr longPos_Ptr, shortPos_Ptr;
				bool isSHFE = newOrder.ExchangeID() == "SHFE";

				if (closeMode)
				{
					longPos_Ptr = _exchangePositionCtx->GetPosition(strategyDO.UserID(), strategyDO.InstrumentID(),
						PositionDirectionType::PD_LONG, strategyDO.PortfolioID());

					shortPos_Ptr = _exchangePositionCtx->GetPosition(strategyDO.UserID(), strategyDO.InstrumentID(),
						PositionDirectionType::PD_SHORT, strategyDO.PortfolioID());
				}

				if (askEnable)
				{
					for (auto pair : askPriceOrders)
					{
						auto askPrice = pair.first;
						int orderVol = pair.second;
						newOrder.Volume = strategyDO.AutoOrderSettings.AskQV - orderVol;

						if (closeMode)
						{
							newOrder.OpenClose = OrderOpenCloseType::CLOSE;
							if (!longPos_Ptr)
							{
								newOrder.Volume = 0;
							}
							else
							{
								int availableYD = longPos_Ptr->LastPosition() - closeVol - orderVol;
								if (isSHFE && availableYD > 0)
								{
									newOrder.OpenClose = OrderOpenCloseType::CLOSEYESTERDAY;
									newOrder.Volume = std::min(newOrder.Volume, availableYD);
								}
								else
								{
									availableYD = longPos_Ptr->Position() - closeVol - orderVol;
									newOrder.Volume = std::min(newOrder.Volume, availableYD);
								}
							}
						}
						else
						{
							newOrder.OpenClose = OrderOpenCloseType::OPEN;
						}

						if (askPrice <= mdo.UpperLimitPrice && newOrder.Volume > 0)
						{
							newOrder.OrderID = 0;
							newOrder.Direction = DirectionType::SELL;
							newOrder.LimitPrice = askPrice.value();

							if (strategyDO.AutoOrderSettings.LimitOrderCounter < strategyDO.AutoOrderSettings.MaxLimitOrder || fakMode)
							{
								newOrder.TIF = strategyDO.AutoOrderSettings.TIF;
								newOrder.VolCondition = strategyDO.AutoOrderSettings.VolCondition;

								if (CreateOrder(newOrder, orderAPI) && newOrder.TIF != OrderTIFType::IOC)
								{
									strategyDO.AutoOrderSettings.LimitOrderCounter++;
								}
							}
						}
					}
				}


				if (bidEnable)
				{
					for (auto pair : bidPriceOrders)
					{
						auto bidPrice = pair.first;
						int orderVol = pair.second;
						newOrder.Volume = strategyDO.AutoOrderSettings.BidQV - orderVol;
						if (closeMode)
						{
							newOrder.OpenClose = OrderOpenCloseType::CLOSE;
							if (!shortPos_Ptr)
							{
								newOrder.Volume = 0;
							}
							else
							{
								int availableYD = shortPos_Ptr->LastPosition() - closeVol - orderVol;
								if (isSHFE && shortPos_Ptr->LastPosition() > 0)
								{
									newOrder.OpenClose = OrderOpenCloseType::CLOSEYESTERDAY;
									newOrder.Volume = std::min(newOrder.Volume, availableYD);
								}
								else
								{
									availableYD = shortPos_Ptr->Position() - closeVol - orderVol;
									newOrder.Volume = std::min(newOrder.Volume, availableYD);
								}
							}
						}
						else
						{
							newOrder.OpenClose = OrderOpenCloseType::OPEN;
						}

						if (bidPrice >= mdo.LowerLimitPrice && newOrder.Volume > 0)
						{
							newOrder.OrderID = 0;
							newOrder.Direction = DirectionType::BUY;
							newOrder.LimitPrice = bidPrice.value();

							if (strategyDO.AutoOrderSettings.LimitOrderCounter < strategyDO.AutoOrderSettings.MaxLimitOrder || fakMode)
							{
								newOrder.TIF = strategyDO.AutoOrderSettings.TIF;
								newOrder.VolCondition = strategyDO.AutoOrderSettings.VolCondition;

								if (CreateOrder(newOrder, orderAPI) && newOrder.TIF != OrderTIFType::IOC)
								{
									strategyDO.AutoOrderSettings.LimitOrderCounter++;
								}
							}
						}
					}
				}
			}
		}

		lock = true;
	});
}

////////////////////////////////////////////////////////////////////////
// Name:       AutoOrderManager::OnStatusChanged(OrderDO orderReq)
// Purpose:    Implementation of AutoOrderManager::OnStatusChanged()
// Parameters:
// - orderReq
// Return:     int
////////////////////////////////////////////////////////////////////////

int AutoOrderManager::OnMarketOrderUpdated(OrderDO& orderInfo, IOrderAPI* orderAPI)
{
	int ret = 0;

	auto orderId = ParseOrderID(orderInfo.OrderID, orderInfo.SessionID, orderAPI);

	bool trigger = false;
	if (auto order_ptr = FindOrder(orderId))
	{
		switch (orderInfo.OrderStatus)
		{
		case OrderStatusType::ALL_TRADED:
		case OrderStatusType::PARTIAL_TRADING:
			ret = orderInfo.VolumeTraded - order_ptr->VolumeTraded;
			order_ptr->VolumeTraded = orderInfo.VolumeTraded;
			trigger = true;
			break;
		case OrderStatusType::CANCELED:
			ret = -1;
			trigger = true;
			break;
		case OrderStatusType::OPEN_REJECTED:
			ret = -2;
			break;
		default:
			ret = 0;
			break;
		}

		order_ptr->Active = orderInfo.Active;
		order_ptr->OrderStatus = orderInfo.OrderStatus;

		if (ret <=0 && !orderInfo.Active)
		{
			_userOrderCtx.RemoveOrder(orderId);
		}

		if (trigger)
		{
			StrategyContractDO_Ptr strategy_ptr;
			if (_pricingCtx->GetStrategyMap()->find(orderInfo, strategy_ptr))
				TradeByStrategy(*strategy_ptr, orderAPI);
		}

		if (ret > 0 && !orderInfo.Active)
		{
			_userOrderCtx.RemoveOrder(orderId);
		}
		
	}

	return ret;
}

uint64_t AutoOrderManager::ParseOrderID(uint64_t rawOrderId, uint64_t sessionId, IOrderAPI* orderAPI)
{
	if (!sessionId)
		sessionId = orderAPI->GetSessionId();

	return sessionId << 32 | rawOrderId;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::CancelOrder(const OrderDO& orderReq)
// Purpose:    Implementation of OTCOrderManager::CancelOrder()
// Parameters:
// - orderReq
// Return:     int
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr AutoOrderManager::CancelOrder(OrderRequestDO& orderReq, IOrderAPI* orderAPI)
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
		OrderContractInnerMapType userOrderMap;
		if (_userOrderCtx.UserOrderMap().find(orderReq.UserID(), userOrderMap))
		{
			OrderIDInnerMapType orders;
			if (userOrderMap.map()->find(orderReq.InstrumentID(), orders))
			{// std::vector<uint64_t> orderCancelList;
				for (auto& pair : orders.map()->lock_table())
				{
					orderAPI->CancelOrder(*pair.second);
					// orderCancelList.push_back(pair.first);
				}

				/*for (auto orderId : orderCancelList)
				_userOrderCtx.RemoveOrder(orderId);*/
			}
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::RejectOrder(const OrderDO& orderReq)
// Purpose:    Implementation of OTCOrderManager::RejectOrder()
// Parameters:
// - orderReq
// Return:     int
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr AutoOrderManager::RejectOrder(OrderRequestDO& orderReq, IOrderAPI* orderAPI)
{
	return CancelOrder(orderReq, orderAPI);
}

int AutoOrderManager::CancelUserOrders(const std::string& userId, IOrderAPI* orderAPI)
{
	OrderContractInnerMapType orderMap;
	if (_userOrderCtx.UserOrderMap().find(userId, orderMap))
	{
		for (auto& it : orderMap.map()->lock_table())
		{
			for (auto& pair : it.second.map()->lock_table())
				CancelOrder(*(pair.second), orderAPI);
		}
		orderMap.map()->clear();
	}

	return 0;
}