#include "ContractCache.h"

static std::vector<InstrumentCache> instrumentCaches(ProductType::PRODUCT_UPPERBOUND);

InstrumentCache & ContractCache::Get(ProductType productType)
{
	return instrumentCaches[productType];
}
