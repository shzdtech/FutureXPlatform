#include "OrderPortfolioCache.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/commonconst.h"


static cuckoohash_map<uint64_t, std::string> orderPortfolioMap(1024);

bool OrderPortfolioCache::Insert(uint64_t orderId, const std::string & portfolio)
{
	return orderPortfolioMap.insert(orderId, portfolio);
}

bool OrderPortfolioCache::Find(uint64_t orderId, std::string& portfolio)
{
	return orderPortfolioMap.find(orderId, portfolio);
}

bool OrderPortfolioCache::Remove(uint64_t orderId)
{
	return orderPortfolioMap.erase(orderId);
}

void OrderPortfolioCache::Clear(void)
{
	orderPortfolioMap.clear();
}
