/***********************************************************************
 * Module:  OTCOrderManager.cpp
 * Author:  milk
 * Modified: 2015年10月22日 23:55:11
 * Purpose: Implementation of the class OTCOrderManager
 ***********************************************************************/

#include "OTCOrderManager.h"
#include "../strategy/PricingContext.h"
#include "../strategy/PricingUtility.h"
#include "../databaseop/OTCOrderDAO.h"


OTCOrderManager::OTCOrderManager(IOrderAPI* pOrderAPI)
	: OrderManager(pOrderAPI)
{

}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::CreateOrder(const OrderDO& orderInfo)
// Purpose:    Implementation of OTCOrderManager::CreateOrder()
// Parameters:
// - orderInfo
// Return:     OrderDO_Ptr
////////////////////////////////////////////////////////////////////////

int OTCOrderManager::CreateOrder(OrderDO& orderInfo)
{
	int ret = -1;

	if (auto pricingDO_ptr = PricingUtility::Pricing(orderInfo, orderInfo.Volume))
	{
		if (auto orderptr = OTCOrderDAO::CreateOrder(orderInfo, *pricingDO_ptr))
		{
			orderInfo = *orderptr;
			if (orderInfo.OrderStatus == OrderStatus::OPENNING)
			{
				std::lock_guard<std::mutex> guard(_userOrderCtx.GetMutex(orderInfo));
				_userOrderCtx.AddOrder(orderInfo);
			}
			ret = 0;
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

OrderDOVec_Ptr OTCOrderManager::UpdateByStrategy(const StrategyContractDO& strategyDO)
{
	OrderDOVec_Ptr ret;

	if (strategyDO.Enabled)
	{
		std::lock_guard<std::mutex> guard(_userOrderCtx.GetMutex(strategyDO));

		auto& tradingMap = _userOrderCtx.GetTradingOrderMap(strategyDO);

		if (tradingMap.size() > 0)
		{
			ret = std::make_shared<VectorDO<OrderDO>>();

			for (auto& it : tradingMap)
			{
				auto& orderDO = *it.second;
				if (auto pricingDO_ptr = PricingUtility::Pricing(strategyDO, orderDO.Volume))
				{
					bool accept = false;
					if (orderDO.Direction == DirectionType::BUY)
					{
						if (orderDO.LimitPrice >= pricingDO_ptr->AskPrice)
							accept = true;
					}
					else
					{
						if (orderDO.LimitPrice <= pricingDO_ptr->BidPrice)
							accept = true;
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
			if (ret->size() > 0)
			{
				Hedge(strategyDO);
			}
		}
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

int OTCOrderManager::CancelOrder(OrderDO& orderInfo)
{
	OrderStatus currStatus;
	OTCOrderDAO::CancelOrder(orderInfo, currStatus);
	orderInfo.OrderStatus = currStatus;
	std::lock_guard<std::mutex> guard(_userOrderCtx.GetMutex(orderInfo));
	_userOrderCtx.RemoveOrder(orderInfo.OrderID);

	return currStatus == OrderStatus::UNDEFINED;

}

////////////////////////////////////////////////////////////////////////
// Name:       OTCOrderManager::RejectOrder(OrderDO orderInfo)
// Purpose:    Implementation of OTCOrderManager::RejectOrder()
// Parameters:
// - orderInfo
// Return:     int
////////////////////////////////////////////////////////////////////////

int OTCOrderManager::RejectOrder(OrderDO& orderInfo)
{
	OrderStatus currStatus;
	OTCOrderDAO::RejectOrder(orderInfo, currStatus);
	orderInfo.OrderStatus = currStatus;
	std::lock_guard<std::mutex> guard(_userOrderCtx.GetMutex(orderInfo));
	_userOrderCtx.RemoveOrder(orderInfo.OrderID);

	return currStatus == OrderStatus::UNDEFINED;

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
	auto& portfolio = PricingContext::GetPortfolioDOMap()->at(portfolioKey);
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
		(portfolioKey,
		&OTCOrderManager::initHedgeOrderMgr,
		this, portfolioKey.UserID());
}


HedgeOrderManager_Ptr OTCOrderManager::initHedgeOrderMgr(const std::string& userID)
{
	return std::make_shared<HedgeOrderManager>(userID, _pOrderAPI);
}