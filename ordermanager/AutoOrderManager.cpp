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


AutoOrderManager::AutoOrderManager(IOrderAPI* pOrderAPI,
	const IPricingDataContext_Ptr& pricingCtx,
	const IUserPositionContext_Ptr& exchangePositionCtx)
	: OrderManager(pOrderAPI, pricingCtx),
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

OrderDO_Ptr AutoOrderManager::CreateOrder(OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;

	if (orderReq.OrderID == 0 || !FindOrder(ParseOrderID(orderReq.OrderID, 0)))
	{
		orderReq.SessionID = _pOrderAPI->GetSessionId();
		orderReq.OrderID = OrderSeqGen::GenOrderID(_pOrderAPI->GetSessionId());
		_contractOrderCtx.UpsertOrder(orderReq.OrderID, orderReq);
		ret = _pOrderAPI->CreateOrder(orderReq);
		if (!ret)
		{
			_contractOrderCtx.RemoveOrder(orderReq.OrderID);
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

void AutoOrderManager::TradeByStrategy(const StrategyContractDO& strategyDO)
{
	if (!strategyDO.Hedging || strategyDO.IsOTC())
		return;

	_updatinglock.upsert(strategyDO, [](bool& lock) { lock = true; }, true);

	_updatinglock.update_fn(strategyDO, [this, &strategyDO](bool& lock)
	{
		cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> orders;

		if (!_contractOrderCtx.ContractOrderMap().find(strategyDO.InstrumentID(), orders))
		{
			_contractOrderCtx.ContractOrderMap().insert(strategyDO.InstrumentID(), cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>(true, strategyDO.Depth * 2));
			_contractOrderCtx.ContractOrderMap().find(strategyDO.InstrumentID(), orders);
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
				mdo.Bid().Price = 0;
				mdo.Ask().Price = 1e32;
			}

			bool closeMode = strategyDO.AutoOrderSettings.CloseMode;
			int depth = closeMode ? 1 : strategyDO.Depth;
			double tickSize = strategyDO.EffectiveTickSize();
			bool askEnable = strategyDO.AskEnabled && strategyDO.AutoOrderSettings.AskCounter < strategyDO.AutoOrderSettings.MaxAutoTrade;
			bool bidEnable = strategyDO.BidEnabled && strategyDO.AutoOrderSettings.BidCounter < strategyDO.AutoOrderSettings.MaxAutoTrade;

			double pricingBidMax = strategyDO.AutoOrderSettings.BidNotCross ? std::min(pricingDO_ptr->Bid().Price, mdo.Bid().Price) : pricingDO_ptr->Bid().Price;
			pricingBidMax = std::floor(pricingBidMax / tickSize) * tickSize;
			double pricingBidMin = pricingBidMax - (depth - 1)*tickSize;

			double pricingAskMin = strategyDO.AutoOrderSettings.BidNotCross ? std::max(pricingDO_ptr->Ask().Price, mdo.Ask().Price) : pricingDO_ptr->Ask().Price;
			pricingAskMin = std::ceil(pricingAskMin / tickSize) * tickSize;
			double pricingAskMax = pricingAskMin + (depth - 1)*tickSize;

			autofillmap<epsdouble, int> bidPriceOrders;
			autofillmap<epsdouble, int> askPriceOrders;

			for (auto& pair : orders.map()->lock_table())
			{
				auto& order = pair.second;
				bool closeType = order->OpenClose != OrderOpenCloseType::OPEN;

				if (order->Direction == DirectionType::SELL)
				{
					if (askEnable &&
						closeMode == closeType &&
						(order->LimitPrice >= pricingAskMin && order->LimitPrice <= pricingAskMax))
					{
						int& totalVol = askPriceOrders.getorfill(order->LimitPrice);
						int remain = order->VolumeRemain();
						if ((totalVol + remain) > strategyDO.AutoOrderSettings.AskQV)
							_pOrderAPI->CancelOrder(*order);
						else
							totalVol += remain;
					}
					else
					{
						_pOrderAPI->CancelOrder(*order);
					}
				}
				else
				{
					if (bidEnable &&
						closeMode == closeType &&
						(order->LimitPrice <= pricingBidMax && order->LimitPrice >= pricingBidMin))
					{
						int& totalVol = bidPriceOrders.getorfill(order->LimitPrice);
						int remain = order->VolumeRemain();
						if ((totalVol + remain) > strategyDO.AutoOrderSettings.BidQV)
							_pOrderAPI->CancelOrder(*order);
						else
							totalVol += remain;
					}
					else
					{
						_pOrderAPI->CancelOrder(*order);
					}
				}
			}

			if (askEnable || bidEnable)
			{

				//for (auto orderId : orderCancelList)
				//	_contractOrderCtx.RemoveOrder(orderId);

				// Make new orders
				OrderRequestDO newOrder(strategyDO, strategyDO.PortfolioID());

				UserPositionExDO_Ptr longPos_Ptr, shortPos_Ptr;
				bool isSHFE = newOrder.ExchangeID() == "SHFE";

				if (closeMode)
				{
					longPos_Ptr = _exchangePositionCtx->GetPosition(strategyDO.UserID(), strategyDO.InstrumentID(),
						PositionDirectionType::PD_LONG, strategyDO.PortfolioID());

					shortPos_Ptr = _exchangePositionCtx->GetPosition(strategyDO.UserID(), strategyDO.InstrumentID(),
						PositionDirectionType::PD_SHORT, strategyDO.PortfolioID());
				}

				epsdouble askPrice(pricingAskMin);
				epsdouble bidPrice(pricingBidMax);

				for (int i = 0; i < depth; i++)
				{
					if (askEnable)
					{
						auto pQV = askPriceOrders.tryfind(askPrice);
						int orderVol = pQV ? *pQV : 0;
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
								int availableYD = longPos_Ptr->LastPosition() - orderVol;
								if (isSHFE && availableYD > 0)
								{
									newOrder.OpenClose = OrderOpenCloseType::CLOSEYESTERDAY;
									newOrder.Volume = std::min(newOrder.Volume, availableYD);
								}
								else
								{
									availableYD = longPos_Ptr->Position() - orderVol;
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

							if (strategyDO.AutoOrderSettings.LimitOrderCounter >= strategyDO.AutoOrderSettings.MaxLimitOrder &&
								strategyDO.AutoOrderSettings.TIF != OrderTIFType::IOC)
							{
								newOrder.TIF = OrderTIFType::IOC;
								newOrder.VolCondition = OrderVolType::ANYVOLUME;
							}
							else
							{
								newOrder.TIF = strategyDO.AutoOrderSettings.TIF;
								newOrder.VolCondition = strategyDO.AutoOrderSettings.VolCondition;
							}

							if (CreateOrder(newOrder) && newOrder.TIF != OrderTIFType::IOC)
							{
								strategyDO.AutoOrderSettings.LimitOrderCounter++;
							}
						}
					}

					if (bidEnable)
					{
						auto pQV = bidPriceOrders.tryfind(bidPrice);
						int orderVol = pQV ? *pQV : 0;
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
								int availableYD = shortPos_Ptr->LastPosition() - orderVol;
								if (isSHFE && shortPos_Ptr->LastPosition() > 0)
								{
									newOrder.OpenClose = OrderOpenCloseType::CLOSEYESTERDAY;
									newOrder.Volume = std::min(newOrder.Volume, availableYD);
								}
								else
								{
									availableYD = shortPos_Ptr->Position() - orderVol;
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

							if (strategyDO.AutoOrderSettings.LimitOrderCounter >= strategyDO.AutoOrderSettings.MaxLimitOrder &&
								strategyDO.AutoOrderSettings.TIF != OrderTIFType::IOC)
							{
								newOrder.TIF = OrderTIFType::IOC;
								newOrder.VolCondition = OrderVolType::ANYVOLUME;
							}
							else
							{
								newOrder.TIF = strategyDO.AutoOrderSettings.TIF;
								newOrder.VolCondition = strategyDO.AutoOrderSettings.VolCondition;
							}

							if (CreateOrder(newOrder) && newOrder.TIF != OrderTIFType::IOC)
							{
								strategyDO.AutoOrderSettings.LimitOrderCounter++;
							}
						}
					}

					askPrice += tickSize;
					bidPrice -= tickSize;
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

int AutoOrderManager::OnMarketOrderUpdated(OrderDO& orderInfo)
{
	int ret = 0;

	auto orderId = ParseOrderID(orderInfo.OrderID, orderInfo.SessionID);

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

		if (!orderInfo.Active)
		{
			_contractOrderCtx.RemoveOrder(orderId);
		}
	}

	return ret;
}

uint64_t AutoOrderManager::ParseOrderID(uint64_t rawOrderId, uint64_t sessionId)
{
	if (!sessionId)
		sessionId = _pOrderAPI->GetSessionId();

	return sessionId << 32 | rawOrderId;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::CancelOrder(const OrderDO& orderReq)
// Purpose:    Implementation of OTCOrderManager::CancelOrder()
// Parameters:
// - orderReq
// Return:     int
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr AutoOrderManager::CancelOrder(OrderRequestDO& orderReq)
{
	OrderDO_Ptr ret;
	if (orderReq.OrderID != 0)
	{
		if (ret = FindOrder(orderReq.OrderID))
		{
			ret = _pOrderAPI->CancelOrder(orderReq);
			// _contractOrderCtx.RemoveOrder(orderReq.OrderID);
		}
	}
	else
	{
		cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> orders;
		if (_contractOrderCtx.ContractOrderMap().find(orderReq.InstrumentID(), orders))
		{
			std::vector<uint64_t> orderCancelList;
			for (auto& pair : orders.map()->lock_table())
			{
				_pOrderAPI->CancelOrder(*pair.second);
				// orderCancelList.push_back(pair.first);
			}

			/*for (auto orderId : orderCancelList)
			_contractOrderCtx.RemoveOrder(orderId);*/
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

OrderDO_Ptr AutoOrderManager::RejectOrder(OrderRequestDO& orderReq)
{
	return CancelOrder(orderReq);
}

int AutoOrderManager::Reset()
{
	auto& orderMap = _contractOrderCtx.GetAllOrder();
	{
		auto lt = orderMap.lock_table();
		for (auto& it : lt)
		{
			CancelOrder(*(it.second));
		}
	}
	_contractOrderCtx.Clear();
	return 0;
}