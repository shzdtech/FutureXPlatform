/***********************************************************************
 * Module:  OTCOrderManager.cpp
 * Author:  milk
 * Modified: 2015年10月22日 23:55:11
 * Purpose: Implementation of the class OTCOrderManager
 ***********************************************************************/

#include "OTCOrderManager.h"
#include "../pricingengine/PricingUtility.h"
#include "../databaseop/OTCOrderDAO.h"


OTCOrderManager::OTCOrderManager(IOrderAPI* pOrderAPI, const IPricingDataContext_Ptr& pricingCtx, IOrderUpdatedEvent* listener)
	: OrderManager(pOrderAPI, pricingCtx, listener),
	_updatinglock(2048)
{ }

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::CreateOrder(const OrderDO& orderInfo)
// Purpose:    Implementation of OTCOrderManager::CreateOrder()
// Parameters:
// - orderInfo
// Return:     OrderDO_Ptr
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr OTCOrderManager::CreateOrder(OrderRequestDO& orderInfo)
{
	OrderDO_Ptr ret;

	StrategyContractDO_Ptr strategy_ptr;
	if (_pricingCtx->GetStrategyMap()->find(orderInfo, strategy_ptr))
	{
		bool reject = false;
		if (orderInfo.Direction != DirectionType::SELL)
		{
			if (!strategy_ptr->BidEnabled)
				reject = true;
		}
		else
		{
			if (!strategy_ptr->AskEnabled)
				reject = true;
		}

		if (reject)
		{
			ret = std::make_shared<OrderDO>(orderInfo);
			ret->OrderStatus = OrderStatusType::OPEN_REJECTED;
		}
		else
		{
			if (!orderInfo.IsOTC()) orderInfo.ConvertToOTC();

			IPricingDO_Ptr pricingDO_ptr;
			if (!_pricingCtx->GetPricingDataDOMap()->find(orderInfo, pricingDO_ptr))
			{
				pricingDO_ptr = PricingUtility::Pricing(nullptr, orderInfo, _pricingCtx);
			}

			if (pricingDO_ptr)
			{
				if (ret = OTCOrderDAO::CreateOrder(orderInfo, *pricingDO_ptr))
				{
					if (ret->OrderStatus == OrderStatusType::OPENED)
					{
						_contractOrderCtx.UpsertOrder(ret->OrderID, *ret);
					}
				}
			}
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::UpdateStatus(const StrategyContractDO& strategyDO)
// Purpose:    Implementation of OTCOrderManager::UpdateStatus()
// Parameters:
// - strategyDO
// Return:     int
////////////////////////////////////////////////////////////////////////

void OTCOrderManager::TradeByStrategy(const StrategyContractDO& strategyDO)
{
	if (!strategyDO.IsOTC())
		return;

	_updatinglock.upsert(strategyDO, [](bool& lock) { lock = true; }, true);

	_updatinglock.update_fn(strategyDO, [this, &strategyDO](bool& lock)
	{
		cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr> orders;

		if (!_contractOrderCtx.ContractOrderMap().find(strategyDO.InstrumentID(), orders))
		{
			_contractOrderCtx.ContractOrderMap().insert(strategyDO.InstrumentID(), cuckoohashmap_wrapper<uint64_t, OrderDO_Ptr>(true, 128));
			_contractOrderCtx.ContractOrderMap().find(strategyDO.InstrumentID(), orders);
		}

		IPricingDO_Ptr pricingDO_ptr;
		if (_pricingCtx->GetPricingDataDOMap()->find(strategyDO, pricingDO_ptr))
		{
			for (auto it : orders.map()->lock_table())
			{
				auto& orderDO = *it.second;
				bool accept = false;
				if (orderDO.Direction != DirectionType::SELL)
				{
					if (strategyDO.BidEnabled)
					{
						//auto pricingDO_ptr = PricingUtility::Pricing(&orderDO.Volume, strategyDO, *_pricingCtx);
						if (orderDO.LimitPrice >= pricingDO_ptr->Ask().Price)
							accept = true;
					}
				}
				else
				{
					if (strategyDO.AskEnabled)
					{
						// auto pricingDO_ptr = PricingUtility::Pricing(&orderDO.Volume, strategyDO, *_pricingCtx);
						if (orderDO.LimitPrice <= pricingDO_ptr->Bid().Price)
							accept = true;
					}
				}

				if (accept)
				{
					OrderStatusType currStatus;
					auto tradeID = OTCOrderDAO::AcceptOrder(orderDO, currStatus);
					if (tradeID > 0)
					{
						_contractOrderCtx.RemoveOrder(orderDO.OrderID);
						orderDO.OrderStatus = currStatus;
						orderDO.VolumeTraded = orderDO.Volume;
						TradeRecordDO_Ptr trade(new TradeRecordDO(orderDO.ExchangeID(), orderDO.InstrumentID(), orderDO.PortfolioID(), orderDO.UserID()));
						trade->Direction = orderDO.Direction;
						trade->Volume = orderDO.Volume;
						trade->Price = orderDO.LimitPrice;
						trade->OpenClose = orderDO.OpenClose;

						GetPositionContext().UpsertPosition(orderDO.UserID(), trade);

						trade->SetPortfolioID(strategyDO.PortfolioID());
						GetPositionContext().UpsertPosition(strategyDO.UserID(), trade);

						if (_listener)
							_listener->OnTraded(trade);
					}
				}
			}
		}
	});
}

OTCUserPositionContext& OTCOrderManager::GetPositionContext()
{
	return _positionCtx;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::OnStatusChanged(OrderDO orderInfo)
// Purpose:    Implementation of OTCOrderManager::OnStatusChanged()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

int OTCOrderManager::OnMarketOrderUpdated(OrderDO& orderInfo)
{
	return -1;
}


////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::CancelOrder(OrderDO orderInfo)
// Purpose:    Implementation of OTCOrderManager::CancelOrder()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr OTCOrderManager::CancelOrder(OrderRequestDO& orderInfo)
{
	if (!orderInfo.IsOTC()) orderInfo.ConvertToOTC();

	OrderStatusType currStatus;
	OTCOrderDAO::CancelOrder(orderInfo, currStatus);
	OrderDO_Ptr ret = _contractOrderCtx.RemoveOrder(orderInfo.OrderID);
	if (ret) ret->OrderStatus = OrderStatusType::CANCELED;

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::RejectOrder(OrderDO orderInfo)
// Purpose:    Implementation of OTCOrderManager::RejectOrder()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

OrderDO_Ptr OTCOrderManager::RejectOrder(OrderRequestDO& orderInfo)
{
	OrderStatusType currStatus;
	OTCOrderDAO::RejectOrder(orderInfo, currStatus);
	OrderDO_Ptr ret = _contractOrderCtx.RemoveOrder(orderInfo.OrderID);
	if (ret)	ret->OrderStatus = OrderStatusType::REJECTED;

	return ret;
}


int OTCOrderManager::Reset()
{
	std::lock_guard<std::mutex> scopelock(_mutex);

	for (auto& it : _contractOrderCtx.GetAllOrder().lock_table())
	{
		RejectOrder(*(it.second));
	}

	//for (auto& it : _hedgeMgr)
	//{
	//	Hedge(it.first);
	//}

	return 0;
}

//void OTCOrderManager::Hedge(const PortfolioKey& portfolioKey)
//{
//	if (auto pPortfolio = _pricingCtx->GetPortfolioMap()->tryfind(portfolioKey))
//	{
//		auto now = std::chrono::steady_clock::now();
//		auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - pPortfolio->LastHedge);
//
//		if (int_ms.count() > pPortfolio->HedgeDelay)
//		{
//			auto addedHedgePos = _positionCtx.GenSpreadPoints(*pPortfolio);
//			auto hedgeMgr_ptr = FindHedgeManager(*pPortfolio);
//			hedgeMgr_ptr->FillPosition(addedHedgePos);
//			hedgeMgr_ptr->Hedge();
//
//			pPortfolio->LastHedge = now;
//		}
//	}
//}

//HedgeOrderManager_Ptr OTCOrderManager::FindHedgeManager(const PortfolioKey& portfolioKey)
//{
//	auto it = _hedgeMgr.find(portfolioKey);
//
//	return (it != _hedgeMgr.end()) ? it->second :
//		_hedgeMgr.getorfillfunc(portfolioKey, &OTCOrderManager::initHedgeOrderMgr, this, portfolioKey);
//}