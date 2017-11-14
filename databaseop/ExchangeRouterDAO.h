#if !defined(__databaseop_ExchangeRouterDAO_h)
#define __databaseop_ExchangeRouterDAO_h

#include <string>
#include <vector>
#include "../dataobject/ExchangeRouterDO.h"
#include "../utility/autofillmap.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS ExchangeRouterDAO
{
public:
	static void FindAllExchangeRouters(std::map<std::string, ExchangeRouterDO>& routerMap);
};

#endif