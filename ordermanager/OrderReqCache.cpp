#include "OrderReqCache.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/commonconst.h"


static cuckoohash_map<uint64_t, OrderRequestDO> orderPortfolioMap(1024);

bool OrderReqCache::Insert(uint64_t orderId, const OrderRequestDO & portfolio)
{
	return orderPortfolioMap.insert(orderId, portfolio);
}

bool OrderReqCache::Find(uint64_t orderId, OrderRequestDO& portfolio)
{
	return orderPortfolioMap.find(orderId, portfolio);
}

bool OrderReqCache::Remove(uint64_t orderId)
{
	return orderPortfolioMap.erase(orderId);
}

void OrderReqCache::Clear(void)
{
	orderPortfolioMap.clear();
}
