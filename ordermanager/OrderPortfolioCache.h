#if !defined(__ordermanager_OrderPortfolioCache_h)
#define __ordermanager_OrderPortfolioCache_h
#include "ordermgr_export.h"
#include <string>

class ORDERMGR_CLASS_EXPORT OrderPortfolioCache
{
public:
	static bool Insert(uint64_t orderId, const std::string& portfolio);
	static bool Find(uint64_t orderId, std::string& portfolio);
	static bool Remove(uint64_t orderId);
	static void Clear(void);
};

#endif