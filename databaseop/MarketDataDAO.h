#if !defined(__databaseop_MarketDataDAO_h)
#define __databaseop_MarketDataDAO_h

#include <vector>
#include <memory>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/MarketDataDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS MarketDataDAO
{
public:
	static bool SaveMarketData(const MarketDataDO& marketDataDO);
	static vector_ptr<MarketDataDO> LoadMarketData(int tradingDay);
};


#endif
