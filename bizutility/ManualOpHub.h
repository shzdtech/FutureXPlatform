#pragma once

#include "../utility/singleton_templ.h"
#include "../utility/lockfree_set.h"
#include "../utility/weak_ptr_hash.h"
#include "bizutility_exp.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/MarketDataDO.h"

class IManualOp
{
public:
	virtual void OnNewManualTrade(const TradeRecordDO& tradeDO) = 0;
	virtual void OnUpdateManualPosition(const UserPositionExDO& positionDO) = 0;
	virtual void OnUpdateMarketData(const MarketDataDO& mdDO) = 0;
};

typedef std::weak_ptr<IManualOp> IManualOp_WkPtr;

class BIZUTILITY_CLASS_EXPORT ManualOpHub : public singleton_ptr<ManualOpHub>
{
public:
	void addListener(const IManualOp_WkPtr& listener);
	void removeListener(const IManualOp_WkPtr& listener);

public:
	void NotifyNewManualTrade(const TradeRecordDO& tradeDO);
	void NotifyUpdateManualPosition(const UserPositionExDO& positionDO);
	void NotifyUpdateMarketData(const MarketDataDO& mdDO);

private:
	lockfree_set<IManualOp_WkPtr, WeakPtrHash<IManualOp>, WeakPtrEqual<IManualOp>> _manualOpHub;
};

