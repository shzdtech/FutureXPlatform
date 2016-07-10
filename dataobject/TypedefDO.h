#if !defined(__dataobject_TypedefDO_h)
#define __dataobject_TypedefDO_h


#include "../utility/autofillmap.h"
#include "ContractKey.h"
#include "ContractDO.h"
#include "PortfolioDO.h"
#include "StrategyContractDO.h"
#include "UserContractParam.h"
#include "MarketDataDO.h"
#include "InstrumentDO.h"
#include "UserPositionDO.h"
#include "EnumTypes.h"

template <typename V>
using ContractMap = typename autofillmap<ContractKey, V>;

template <typename V>
using ContractKeyMap = typename std::map<ContractKey, V>;

template <typename V>
using UserContractMap = typename autofillmap<UserContractKey, V>;

//template <typename V>
//using User_ContractMap = typename autofillmap<std::string, ContractMap<V>>;

typedef ContractMap<StrategyContractDO> StrategyContractDOMap;

typedef autofillmap<std::string, MarketDataDO> MarketDataDOMap;

typedef ContractMap<UserContractParam> UserContractParamMap;

typedef ContractMap<ContractDO> ContractDOMap;

typedef std::map<std::string, InstrumentDO> InstrumentDOMap;

typedef autofillmap<std::string, autofillmap<PositionDirectionType, UserPositionExDO>>
UserPositionExDOMap;

template <typename V>
using PortfolioMap = typename autofillmap<PortfolioKey, V>;

typedef PortfolioMap<PortfolioDO> PortfolioDOMap;


#endif