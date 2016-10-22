#include "TestingQueryInstruments.h"
#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"
#include "sqlite3.h"
#include <mutex>

bool TestingQueryInstruments::_loaded = false;

static int callback(void *NotUsed, int count, char **data, char **columns)
{
	auto& cache = ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE);
	for (int i = 0; i < count; i++) {
		InstrumentDO insDO(data[0], data[1]);
		insDO.Name = data[2];
		insDO.ProductID = data[3];
		insDO.ProductType = (ProductType)std::stoi(data[4]);
		insDO.VolumeMultiple = std::stod(data[5]);
		insDO.PriceTick = std::stod(data[6]);
		if (data[7]) DateType::YYYYMMDD2YYYY_MM_DD(data[7], insDO.ExpireDate);
		insDO.StrikePrice = std::stod(data[8]);
		insDO.ContractType = (ContractType)std::stoi(data[9]);
		insDO.UnderlyingContract = ContractKey(data[10], data[11]);
		cache.Add(insDO);
	}
	return 0;
}

dataobj_ptr TestingQueryInstruments::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	if (!_loaded)
	{
		static std::mutex mutex;
		std::lock_guard<std::mutex> _lock(mutex);
		if (!_loaded)
		{
			sqlite3 *db;
			char *zErrMsg;
			int rc = sqlite3_open("clientcache.db", &db);
			if (!rc) {
				char* sql = "select exchange,contract,name,productID,productType,volumeMultiple,priceTick,expireDate,strikePrice,contractType,underlyingExchange,underlyingContract from contractinfo";
				rc = sqlite3_exec(db, sql, callback, nullptr, &zErrMsg);
				if (rc)
					LOG_ERROR << zErrMsg;
			}
			sqlite3_close(db);
			_loaded = true;
		}
	}

	return ContractCache::Get(ProductCacheType::PRODUCT_CACHE_EXCHANGE).AllInstruments();
}