#if !defined(__dataobject_TypedefDO_h)
#define __dataobject_TypedefDO_h

#include "../utility/autofillmap.h"
#include "../utility/stringutility.h"
#include "ContractKey.h"
#include "ContractParamDO.h"
#include "PortfolioDO.h"
#include "StrategyContractDO.h"
#include "UserContractParamDO.h"
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

typedef autofillmap<std::string, MarketDataDO, ci_less> MarketDataDOMap;

typedef ContractMap<UserContractParamDO> UserContractParamDOMap;

typedef ContractMap<ContractParamDO> ContractParamDOMap;

typedef autofillmap<std::string, InstrumentDO, ci_less> InstrumentDOMap;

typedef autofillmap<std::string, autofillmap<std::pair<PositionDateFlagType, PositionDirectionType>, UserPositionExDO>, ci_less> UserPositionExDOMap;

template <typename V>
using PortfolioMap = typename autofillmap<PortfolioKey, V>;

typedef PortfolioMap<PortfolioDO> PortfolioDOMap;


#endif