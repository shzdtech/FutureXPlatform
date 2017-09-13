#include "ManualOpHub.h"
#include "../utility/scopeunlock.h"

void ManualOpHub::addListener(const IManualOp_WkPtr & listener)
{
	_manualOpHub.emplace(listener);
}

void ManualOpHub::removeListener(const IManualOp_WkPtr & listener)
{
	_manualOpHub.erase(listener);
}

void ManualOpHub::NotifyNewManualTrade(const TradeRecordDO & tradeDO)
{
	scopeunlock<lockfree_set<IManualOp_WkPtr, WeakPtrHash<IManualOp>, WeakPtrEqual<IManualOp>>> lock(&_manualOpHub);

	for (auto mop : _manualOpHub.rawset())
	{
		if (auto mop_ptr = mop.lock())
		{
			mop_ptr->OnNewManualTrade(tradeDO);
		}
	}
}

void ManualOpHub::NotifyUpdateManualPosition(const UserPositionExDO & positionDO)
{
	scopeunlock<lockfree_set<IManualOp_WkPtr, WeakPtrHash<IManualOp>, WeakPtrEqual<IManualOp>>> lock(&_manualOpHub);

	for (auto mop : _manualOpHub.rawset())
	{
		if (auto mop_ptr = mop.lock())
		{
			mop_ptr->OnUpdateManualPosition(positionDO);
		}
	}
}
