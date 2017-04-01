#if !defined(__dataobject_TypedefDO_h)
#define __dataobject_TypedefDO_h

#include "../include/libcuckoo/cuckoohash_map.hh"
#include "../utility/autofillmap.h"
#include "../utility/pairhash.h"
#include "../utility/stringutility.h"
#include "ContractKey.h"
#include "ContractParamDO.h"
#include "PortfolioDO.h"
#include "StrategyContractDO.h"
#include "UserContractParamDO.h"
#include "MarketDataDO.h"
#include "InstrumentDO.h"
#include "UserPositionDO.h"
#include "TemplateDO.h"
#include "EnumTypes.h"

template <typename V>
using ContractMap = typename autofillmap<ContractKey, V>;

template <typename V>
using ContractKeyMap = typename std::map<ContractKey, V>;

template <typename V>
using UserContractMap = typename autofillmap<UserContractKey, V>;

//template <typename V>
//using User_ContractMap = typename autofillmap<std::string, ContractMap<V>>;

template <typename T>
using TContractMap = typename cuckoohash_map<ContractKey, T, ContractKeyHash>;

typedef TContractMap<StrategyContractDO_Ptr> StrategyContractDOMap;

typedef autofillmap<std::string, autofillmap<std::string, std::shared_ptr<StrategyContractDOMap>>> UserStrategyMap;

typedef cuckoohash_map<std::string, MarketDataDO> MarketDataDOMap;

typedef TContractMap<IPricingDO_Ptr> PricingDataDOMap;

typedef ContractMap<UserContractParamDO> UserContractParamDOMap;

typedef ContractMap<ContractParamDO> ContractParamDOMap;

typedef autofillmap<std::string, InstrumentDO, ci_less> InstrumentDOMap;

typedef autofillmap<std::string, autofillmap<std::pair<PositionDateFlagType, PositionDirectionType>, UserPositionExDO>, ci_less> UserPositionExDOMap;

template <typename V>
using PortfolioMap = typename autofillmap<PortfolioKey, V>;

typedef PortfolioMap<PortfolioDO> PortfolioDOMap;

typedef VectorDO<ContractKey> ContractList;

#endif