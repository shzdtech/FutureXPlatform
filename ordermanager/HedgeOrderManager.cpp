/***********************************************************************
 * Module:  HedgeOrderManager.cpp
 * Author:  milk
 * Modified: 2016年1月15日 23:39:13
 * Purpose: Implementation of the class HedgeOrderManager
 ***********************************************************************/

#include "HedgeOrderManager.h"
#include "../utility/atomicutil.h"
#include "../utility/epsdouble.h"

HedgeOrderManager::HedgeOrderManager(const PortfolioKey& portfolio,
	IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx)
	: _portfolio(portfolio), AutoOrderManager(pOrderAPI, pricingCtx),
	_contractFlag(1024), _contractPosition(1024)
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
		case OrderStatusType::ALL_TRADED:
		case OrderStatusType::CANCELED:
		{
			int delta = orderInfo.VolumeTraded - order_ptr->VolumeTraded;
			if (delta > 0)
			{
				_mktPosCtx.UpdatePosition(orderInfo,
					(DirectionType)orderInfo.Direction,
					delta);

				if (orderInfo.Direction == DirectionType::SELL)
					delta = -delta;

				double remain = 0;
				_contractPosition.upsert(orderInfo, [&remain, delta](double& pos)
				{ remain = (pos += delta); }, delta);

				if (remain <= -1 || remain >= 1)
				{
					OrderRequestDO newOrder(orderInfo);
					newOrder.Volume = std::labs(remain);
					std::shared_ptr<OrderRequestDO> req1;
					std::shared_ptr<OrderRequestDO> req2;
					SplitOrders(newOrder, req1, req2);
					if (req1) CreateOrder(*req1);
					if (req2) CreateOrder(*req2);
				}
				else
				{
					_contractFlag.update(orderInfo, false);
				}
			}
		}
		break;
		default:
			break;
		}

		if (!orderInfo.Active)
		{
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
	auto lt = _contractPosition.lock_table();
	for (auto& it : lt)
	{
		double position = it.second;
		if (position <= -1 || position >= 1)
		{
			bool hedging;
			_contractFlag.update_fn(it.first, [&hedging](bool& h) { hedging = h; h = true; });
			if (!hedging) //if not starting hedging for this contract
			{
				// Make new orders
				OrderRequestDO newOrder(0, it.first.ExchangeID(), it.first.InstrumentID(),
					_portfolio.UserID(), _portfolio.PortfolioID());
				newOrder.Volume = std::labs(position);
				newOrder.Direction = position < 0 ? DirectionType::SELL : DirectionType::BUY;
				std::shared_ptr<OrderRequestDO> req1;
				std::shared_ptr<OrderRequestDO> req2;
				SplitOrders(newOrder, req1, req2);
				if (req1) CreateOrder(*req1);
				if (req2) CreateOrder(*req2);
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


void HedgeOrderManager::FillPosition(const ContractMap<double>& positionMap)
{
	for (auto& it : positionMap)
	{
		double position = it.second;
		_contractPosition.upsert(it.first,
			[position](double& pos) { pos += position; }, position);
	}
}

void HedgeOrderManager::SplitOrders(const OrderRequestDO & orderInfo, std::shared_ptr<OrderRequestDO>& req1, std::shared_ptr<OrderRequestDO>& req2)
{
	auto pMdMap = _pricingCtx->GetMarketDataMap();
	if (auto pMdo = pMdMap->tryfind(orderInfo.InstrumentID()))
	{
		req1.reset(new OrderRequestDO(orderInfo));
		req1->TIF = OrderTIFType::IOC;

		int pos = _mktPosCtx.GetPosition(orderInfo);
		if (orderInfo.Direction == DirectionType::SELL && pos > 0)
		{
			req1->LimitPrice = pMdo->Ask().Price;
			req1->OpenClose = OrderOpenCloseType::CLOSE;
			if (pos < orderInfo.Volume)
			{
				req1->Volume = pos;
				req2.reset(new OrderRequestDO(*req1));
				req2->OpenClose = OrderOpenCloseType::OPEN;
				req2->Volume = orderInfo.Volume - pos;
			}
		}
		else if (orderInfo.Direction != DirectionType::SELL && pos < 0)
		{
			req1->LimitPrice = pMdo->Bid().Price;
			pos = -pos;
			req1->OpenClose = OrderOpenCloseType::CLOSE;
			if (pos < orderInfo.Volume)
			{
				req1->Volume = pos;
				req2.reset(new OrderRequestDO(*req1));
				req2->OpenClose = OrderOpenCloseType::OPEN;
				req2->Volume = orderInfo.Volume - pos;
			}
		}
		else
		{
			req2.reset();
		}
	}
}
