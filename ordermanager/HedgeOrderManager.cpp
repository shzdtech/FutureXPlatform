/***********************************************************************
 * Module:  HedgeOrderManager.cpp
 * Author:  milk
 * Modified: 2016年1月15日 23:39:13
 * Purpose: Implementation of the class HedgeOrderManager
 ***********************************************************************/

#include "HedgeOrderManager.h"
#include "../utility/atomicutil.h"
#include "../utility/epsdouble.h"

HedgeOrderManager::HedgeOrderManager(const std::string& user,
	IOrderAPI* pOrderAPI, IPricingDataContext* pricingCtx)
	: _user(user), AutoOrderManager(pOrderAPI, pricingCtx)
{

}

////////////////////////////////////////////////////////////////////////
// Name:       HedgeOrderManager::CreateOrder(const OrderDO& orderInfo)
// Purpose:    Implementation of AutoOrderManager::CreateOrder()
// Parameters:
// - orderInfo
// Return:     OrderDO_Ptr
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr HedgeOrderManager::CreateOrder(OrderRequestDO& orderInfo)
{
	auto pMdMap = _pricingCtx->GetMarketDataMap();
	auto& mdo = pMdMap->at(orderInfo.InstrumentID());

	orderInfo.TIF = OrderTIFType::IOC;

	if (orderInfo.Direction == DirectionType::SELL)
	{
		orderInfo.LimitPrice = mdo.AskPrice;

		int pos = _mktPosCtx.GetBuyPosition(orderInfo);
		if (pos >= orderInfo.Volume)
		{
			orderInfo.OpenClose = CLOSE;
		}
	}
	else
	{
		orderInfo.LimitPrice = mdo.BidPrice;

		int pos = _mktPosCtx.GetSellPosition(orderInfo);
		if (pos >= orderInfo.Volume)
		{
			orderInfo.OpenClose = CLOSE;
		}
	}

	return AutoOrderManager::CreateOrder(orderInfo);
}


////////////////////////////////////////////////////////////////////////
// Name:       HedgeOrderManager::UpdateByStrategy(const StrategyContractDO& strategyDO)
// Purpose:    Implementation of HedgeOrderManager::UpdateByStrategy()
// Parameters:
// - strategyDO
// Return:     OrderDOVec_Ptr
////////////////////////////////////////////////////////////////////////

OrderDOVec_Ptr HedgeOrderManager::UpdateOrderByStrategy(
	const StrategyContractDO& strategyDO)
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       HedgeOrderManager::OnOrderUpdated(OrderDO& orderInfo)
// Purpose:    Implementation of HedgeOrderManager::OnOrderUpdated()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

int HedgeOrderManager::OnOrderUpdated(OrderDO& orderInfo)
{
	int ret = -1;

	if (auto order_ptr = FindOrder(orderInfo.OrderID))
	{
		switch (orderInfo.OrderStatus)
		{
		case OrderStatus::ALL_TRADED:
		case OrderStatus::CANCELED:
		{
			int delta = orderInfo.VolumeTraded - order_ptr->VolumeTraded;
			if (delta > 0)
			{
				_mktPosCtx.UpdatePosition(orderInfo, 
					(DirectionType)orderInfo.Direction, 
					(OrderOpenCloseType)orderInfo.OpenClose, delta);

				if (orderInfo.Direction == DirectionType::SELL)
					delta = -delta;

				double zero = 0;
				auto& position = _contractPosition.getorfill(orderInfo, zero);
				double remain = atomicutil::atomic_fetch_add(position, (double)delta);
				if (remain <= -1 || remain >= 1)
				{
					OrderRequestDO newOrder(orderInfo);
					newOrder.Volume = std::labs(remain);
					CreateOrder(newOrder);
				}
				else
				{
					_contractMutex.getorfill(orderInfo).mutex().unlock();
				}
			}
		}
			break;
		default:
			break;
		}

		if (!orderInfo.Active)
		{
			std::lock_guard<std::mutex> guard(_userOrderCtx.Mutex(orderInfo));
			_userOrderCtx.RemoveOrder(orderInfo.OrderID);
		}

		ret = 0;
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       HedgeOrderManager::Hedge()
// Purpose:    Implementation of HedgeOrderManager::Hedge()
// Return:     int
////////////////////////////////////////////////////////////////////////

int HedgeOrderManager::Hedge(void)
{
	for (auto& it : _contractPosition)
	{
		double position = it.second;
		if (position <= -1 || position >= 1)
		{
			if (_contractMutex.getorfill(it.first).mutex().try_lock()) //if hedging for this contract
			{
				// Make new orders
				OrderRequestDO newOrder(0, it.first.ExchangeID(), it.first.InstrumentID(), _user);
				newOrder.Volume = std::labs(position);
				newOrder.Direction = position < 0 ? DirectionType::SELL : DirectionType::BUY;
				CreateOrder(newOrder);
			}
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////
// Name:       HedgeOrderManager::Reset()
// Purpose:    Implementation of HedgeOrderManager::Reset()
// Return:     int
////////////////////////////////////////////////////////////////////////

int HedgeOrderManager::Reset(void)
{
	return 0;
}


void HedgeOrderManager::FillPosition(ContractMap<double>& position)
{
	for (auto& it : position)
	{
		double zero = 0;
		auto& position = _contractPosition.getorfill(it.first, zero);
		atomicutil::atomic_fetch_add(position, it.second);
	}
}