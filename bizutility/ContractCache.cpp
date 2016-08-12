#include "ContractCache.h"


InstrumentCache & ContractCache::Get(ProductType productType)
{
	static std::vector<InstrumentCache> instrumentCaches(ProductType::PRODUCT_UPPERBOUND);

	return instrumentCaches[productType];
}
