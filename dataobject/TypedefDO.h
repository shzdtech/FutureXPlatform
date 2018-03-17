#if !defined(__dataobject_TypedefDO_h)
#define __dataobject_TypedefDO_h

#include "../utility/cuckoohashmap_wrapper.h"
#include "../utility/autofillmap.h"
#include "../utility/pairhash.h"
#include "../utility/stringutility.h"
#include "../utility/lockfree_set.h"
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
#include "ModelParamsDO.h"

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

template <typename T>
using TUserContractMap = typename cuckoohash_map<UserContractKey, T, UserContractKeyHash>;

typedef TUserContractMap<StrategyContractDO_Ptr> StrategyContractDOMap;

typedef autofillmap<std::string, autofillmap<std::string, std::shared_ptr<StrategyContractDOMap>>> UserStrategyMap;

typedef cuckoohash_map<std::string, MarketDataDO> MarketDataDOMap;

typedef TContractMap<IPricingDO_Ptr> PricingDataDOMap;

typedef ContractMap<UserContractParamDO> UserContractParamDOMap;

typedef ContractMap<ContractParamDO> ContractParamDOMap;

typedef autofillmap<std::string, InstrumentDO, ci_less> InstrumentDOMap;

typedef autofillmap<std::string, autofillmap<std::pair<PositionDateFlagType, PositionDirectionType>, UserPositionExDO>, ci_less> UserPositionExDOMap;

typedef autofillmap<std::string, autofillmap<std::string, PortfolioDO>> PortfolioDOMap;

typedef VectorDO<ContractKey> ContractList;

typedef cuckoohashmap_wrapper<std::string, ModelKey> UserModelKeyMap;
typedef cuckoohash_map<std::string, UserModelKeyMap> ModelKeyMap;

#endif