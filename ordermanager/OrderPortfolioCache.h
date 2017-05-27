#if !defined(__ordermanager_OrderPortfolioCache_h)
#define __ordermanager_OrderPortfolioCache_h
#include "ordermgr_export.h"
#include "../dataobject/PortfolioDO.h"

class ORDERMGR_CLASS_EXPORT OrderPortfolioCache
{
public:
	static bool Insert(uint64_t orderId, const PortfolioKey& portfolio);
	static bool Find(uint64_t orderId, PortfolioKey& portfolio);
	static bool Remove(uint64_t orderId);
	static void Clear(void);
};

#endif