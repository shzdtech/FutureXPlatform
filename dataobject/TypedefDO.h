#if !defined(__dataobject_TypedefDO_h)
#define __dataobject_TypedefDO_h


#include "../utility/AutoFillMap.h"
#include "ContractKey.h"
#include "ContractDO.h"
#include "PortfolioDO.h"
#include "StrategyContractDO.h"
#include "UserContractParam.h"
#include "MarketDataDO.h"

template <typename V>
using ContractMap = typename AutoFillMap<ContractKey, V>;

template <typename V>
using ContractKeyMap = typename std::map<ContractKey, V>;

template <typename V>
using UserContractMap = typename AutoFillMap<UserContractKey, V>;

//template <typename V>
//using User_ContractMap = typename AutoFillMap<std::string, ContractMap<V>>;

typedef ContractMap<StrategyContractDO> StrategyContractDOMap;

typedef std::map<std::string, MarketDataDO> MarketDataDOMap;

typedef ContractMap<UserContractParam> UserContractParamMap;

typedef ContractMap<ContractDO> ContractDOMap;

template <typename V>
using PortfolioMap = typename AutoFillMap<PortfolioKey, V>;

typedef PortfolioMap<PortfolioDO> PortfolioDOMap;

#endif