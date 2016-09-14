#include "ContractCache.h"

static std::vector<InstrumentCache> instrumentCaches(ProductCacheType::PRODUCT_CACHE_UPPERBOUND);
static std::map<std::string, InstrumentSyncFlag> instrumentCacheFlag;

InstrumentCache & ContractCache::Get(ProductCacheType productCacheType)
{
	return instrumentCaches[productCacheType];
}

void ContractCache::SetSyncFlag(const std::string & datasource, InstrumentSyncFlag flag)
{
	instrumentCacheFlag[datasource] = flag;
}

InstrumentSyncFlag ContractCache::GetSyncFlag(const std::string & datasource)
{
	InstrumentSyncFlag ret = InstrumentSyncFlag::INSRUMENT_INFO_NOT_SYNC;
	auto it = instrumentCacheFlag.find(datasource);
	if (it != instrumentCacheFlag.end())
		ret = it->second;

	return ret;
}
