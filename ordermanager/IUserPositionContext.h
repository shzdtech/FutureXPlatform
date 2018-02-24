#pragma once

#include "../dataobject/UserPositionDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/cuckoohashmap_wrapper.h"
#include "../utility/pairhash.h"
#include "../common/typedefs.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/RiskDO.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/ValuationRiskDO.h"
#include "../pricingengine/IPricingDataContext.h"

typedef cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>> ContractPosition;
typedef cuckoohashmap_wrapper<std::string, ContractPosition> PortfolioPosition;
typedef cuckoohash_map<std::string, PortfolioPosition> UserPortfolioPosition;


typedef cuckoohashmap_wrapper<std::string, PositionPnLDO_Ptr> ContractPositionPnL;
typedef cuckoohashmap_wrapper<std::string, ContractPositionPnL> PortfolioPositionPnL;
typedef cuckoohash_map<std::string, PortfolioPositionPnL> UserPortfolioPositionPnL;

typedef autofillmap<std::string, autofillmap<ContractKey, RiskDO>> UnderlyingRiskMap;
typedef cuckoohash_map<uint128, bool, UINT128Hash> PositionTradeIDMapType;

class IUserPositionContext
{
public:
	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO, 
		bool updateYdPosition = false, bool closeYdFirst = false, bool forceSync = false) = 0;

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userid, const TradeRecordDO_Ptr & tradeDO, int multiplier = 1, bool closeYdFirst = false) = 0;

	virtual int UpdatePnL(const std::string& userID, const MarketDataDO& mdDO) = 0;

	virtual void Clear(void) = 0;

	virtual PortfolioPositionPnL GetPortfolioPositionsPnLByUser(const std::string& userID) = 0;

	virtual ContractPositionPnL GetPositionsPnLByUser(const std::string& userID, const std::string& portfolio = "") = 0;

	virtual PositionPnLDO_Ptr GetPositionPnL(const std::string& userID, const std::string& instumentID, const std::string& portfolio = "") = 0;

	virtual PortfolioPosition GetPortfolioPositionsByUser(const std::string& userID) = 0;

	virtual ContractPosition GetPositionsByUser(const std::string& userID, const std::string& portfolio = "") = 0;

	virtual UserPositionExDO_Ptr GetPosition(const std::string& userID, const std::string& instumentID, PositionDirectionType direction, const std::string& portfolio = "") = 0;

	virtual bool RemoveUserPosition(const std::string & userID) = 0;

	virtual bool RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio = "") = 0;

	virtual bool AllPosition(std::vector<UserPositionExDO_Ptr>& positions) = 0;

	virtual bool GetRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx, const std::string& userID, const std::string& portfolio, UnderlyingRiskMap& risks) = 0;

	virtual bool GetValuationRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
		const std::string & userID, const ValuationRiskDO& valuationRisk, UnderlyingRiskMap& risks) = 0;

	virtual bool ContainsTrade(uint128 tradeID) = 0;
};

typedef std::shared_ptr<IUserPositionContext> IUserPositionContext_Ptr;