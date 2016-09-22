/***********************************************************************
 * Module:  OTCOrderManager.cpp
 * Author:  milk
 * Modified: 2015年10月22日 23:55:11
 * Purpose: Implementation of the class OTCOrderManager
 ***********************************************************************/

#include "OTCOrderManager.h"
#include "../pricingengine/PricingUtility.h"
#include "../databaseop/OTCOrderDAO.h"


OTCOrderManager::OTCOrderManager(IOrderAPI* pOrderAPI, IPricingDataContext* pricingCtx)
	: OrderManager(pOrderAPI, pricingCtx),
	_positionCtx(pricingCtx)
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

	auto pStrategyMap = _pricingCtx->GetStrategyMap();

	if (auto pStrategy = pStrategyMap->tryfind(orderInfo))
	{
		bool reject = false;
		if (orderInfo.Direction == DirectionType::BUY)
		{
			if (!pStrategy->BidEnabled)
				reject = true;
		}
		else
		{
			if (!pStrategy->AskEnabled)
				reject = true;
		}

		if (reject)
		{
			ret = std::make_shared<OrderDO>(orderInfo);
			ret->OrderStatus = OrderStatus::OPEN_REJECTED;
		}
		else
		{
			if (!orderInfo.IsOTC()) orderInfo.ConvertToOTC();

			if (auto pricingDO_ptr = PricingUtility::Pricing(&orderInfo.Volume, orderInfo, *_pricingCtx))
			{
				if (ret = OTCOrderDAO::CreateOrder(orderInfo, *pricingDO_ptr))
				{
					if (ret->OrderStatus == OrderStatus::OPENED)
					{
						_userOrderCtx.AddOrder(*ret);
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

OrderDOVec_Ptr OTCOrderManager::UpdateOrderByStrategy(const StrategyContractDO& strategyDO)
{
	OrderDOVec_Ptr ret;

	std::lock_guard<std::shared_mutex> guard(_userOrderCtx.UserMutex(strategyDO.UserID()));

	auto& tradingMap = _userOrderCtx.GetOrderMapByUserContract(strategyDO);

	if (!tradingMap.empty())
	{
		ret = std::make_shared<VectorDO<OrderDO>>();

		for (auto& it : tradingMap)
		{
			auto& orderDO = *it.second;
			bool accept = false;
			if (orderDO.Direction == DirectionType::BUY)
			{
				if (strategyDO.BidEnabled)
				{
					auto pricingDO_ptr = PricingUtility::Pricing(&orderDO.Volume, strategyDO, *_pricingCtx);
					if (pricingDO_ptr && orderDO.LimitPrice >= pricingDO_ptr->Ask().Price)
						accept = true;
				}
			}
			else
			{
				if (strategyDO.AskEnabled)
				{
					auto pricingDO_ptr = PricingUtility::Pricing(&orderDO.Volume, strategyDO, *_pricingCtx);
					if (pricingDO_ptr && orderDO.LimitPrice <= pricingDO_ptr->Bid().Price)
						accept = true;
				}
			}

			if (accept)
			{
				OrderStatus currStatus;
				if (OTCOrderDAO::AcceptOrder(orderDO, currStatus))
				{
					orderDO.OrderStatus = currStatus;
					orderDO.VolumeTraded = orderDO.Volume;
					orderDO.VolumeRemain = 0;
					ret->push_back(orderDO);
					_positionCtx.UpdatePosition(strategyDO,
						(DirectionType)orderDO.Direction,
						(OrderOpenCloseType)orderDO.OpenClose,
						orderDO.Volume);
					_userOrderCtx.RemoveOrder(orderDO.OrderID);
				}
			}
		}
	}

	if (ret && ret->size() > 0)
	{
		Hedge(strategyDO);
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::OnStatusChanged(OrderDO orderInfo)
// Purpose:    Implementation of OTCOrderManager::OnStatusChanged()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

int OTCOrderManager::OnOrderUpdated(OrderDO& orderInfo)
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

	OrderStatus currStatus;
	OTCOrderDAO::CancelOrder(orderInfo, currStatus);
	OrderDO_Ptr ret = _userOrderCtx.RemoveOrder(orderInfo.OrderID);
	if (ret) ret->OrderStatus = OrderStatus::CANCELED;

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
	OrderStatus currStatus;
	OTCOrderDAO::RejectOrder(orderInfo, currStatus);
	OrderDO_Ptr ret = _userOrderCtx.RemoveOrder(orderInfo.OrderID);
	if (ret)	ret->OrderStatus = OrderStatus::REJECTED;

	return ret;

}


int OTCOrderManager::Reset()
{
	std::lock_guard<std::mutex> scopelock(_mutex);

	auto& orderMap = _userOrderCtx.GetAllOrder();
	for (auto& it : orderMap)
	{
		RejectOrder(*(it.second));
	}

	for (auto& it : _hedgeMgr)
	{
		Hedge(it.first);
	}

	return 0;
}

void OTCOrderManager::Hedge(const PortfolioKey& portfolioKey)
{
	auto& portfolio = _pricingCtx->GetPortfolioMap()->at(portfolioKey);
	auto now = std::chrono::steady_clock::now();
	auto int_ms =
		std::chrono::duration_cast<std::chrono::milliseconds>
		(now - portfolio.LastHedge);

	if (int_ms.count() > portfolio.HedgeDelay)
	{
		auto addedHedgePos = _positionCtx.GenSpreadPoints(portfolio, true);
		auto hedgeMgr_ptr = FindHedgeManager(portfolio);
		hedgeMgr_ptr->FillPosition(addedHedgePos);
		hedgeMgr_ptr->Hedge();

		portfolio.LastHedge = now;
	}
}

HedgeOrderManager_Ptr OTCOrderManager::FindHedgeManager(const PortfolioKey& portfolioKey)
{
	auto it = _hedgeMgr.find(portfolioKey);

	return (it != _hedgeMgr.end()) ? it->second : _hedgeMgr.getorfillfunc
	(portfolioKey, &OTCOrderManager::initHedgeOrderMgr, this, portfolioKey);
}


HedgeOrderManager_Ptr OTCOrderManager::initHedgeOrderMgr(const PortfolioKey& portfolio)
{
	return std::make_shared<HedgeOrderManager>(portfolio, _pOrderAPI, _pricingCtx);
}