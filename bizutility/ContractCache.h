#pragma once
#include "InstrumentCache.h"
#include <string>
#include "bizutility_exp.h"


enum ProductCacheType
{
	PRODUCT_CACHE_EXCHANGE = 0,
	PRODUCT_CACHE_OTC_CONTRACT = 1,
	PRODUCT_CACHE_OTC_OPTION = 2,
	PRODUCT_CACHE_UPPERBOUND,
};

enum InstrumentSyncFlag
{
	INSRUMENT_INFO_NOT_SYNC = 0,
	INSRUMENT_INFO_SYNCING = 1,
	INSRUMENT_INFO_SYNCED = 2
};


class BIZUTILITY_CLASS_EXPORT ContractCache
{
public:
	static InstrumentCache& Get(ProductCacheType productCacheType);
	static void SetSyncFlag(const std::string& datasource, InstrumentSyncFlag flag);
	static InstrumentSyncFlag GetSyncFlag(const std::string& datasource);
};

