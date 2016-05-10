/***********************************************************************
 * Module:  HedgeOrderManager.h
 * Author:  milk
 * Modified: 2016年1月15日 23:39:13
 * Purpose: Declaration of the class HedgeOrderManager
 ***********************************************************************/

#if !defined(__ordermanager_HedgeOrderManger_h)
#define __ordermanager_HedgeOrderManger_h

#include "AutoOrderManager.h"
#include "MarketPositionContext.h"
#include "../dataobject/TypedefDO.h"


#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT HedgeOrderManager : public AutoOrderManager
{
public:
	HedgeOrderManager(const std::string& user, IOrderAPI* pOrderAPI, PricingContext* pricingCtx);

	int CreateOrder(OrderDO& orderInfo);

	int OnOrderUpdated(OrderDO& orderInfo);

	int Hedge(void);

	int Reset(void);

	OrderDOVec_Ptr UpdateOrderByStrategy(const StrategyContractDO& strategyDO);

	void FillPosition(ContractMap<double>& position);

protected:
	const std::string& _user;
	ContractMap<MovableMutex> _contractMutex;
	ContractMap<std::atomic<double>> _contractPosition;
	MarketPositionContext _mktPosCtx;

private:

};

typedef std::shared_ptr<HedgeOrderManager> HedgeOrderManager_Ptr;

#endif