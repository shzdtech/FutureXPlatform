#include "ContractCache.h"
#include "../databaseop/VersionDAO.h"
#include "../databaseop/ContractDAO.h"

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

bool ContractCache::PersistCache(ProductCacheType productCacheType, const std::string & datasource, const std::string & version)
{
	std::string ver;
	VersionDAO::GetVersion(datasource, ver);
	if (ver != version)
	{
		auto vector_ptr = ContractCache::Get(productCacheType).AllInstruments();
		ContractDAO::UpsertContracts(*vector_ptr);
		VersionDAO::UpsertVersion(datasource, version);
	}
}