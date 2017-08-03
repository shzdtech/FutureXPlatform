/***********************************************************************
 * Module:  OTCUserOrderContext.h
 * Author:  milk
 * Modified: 2016年1月26日 17:31:32
 * Purpose: Declaration of the class OTCUserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_OTCUserOrderContext_h)
#define __ordermanager_OTCUserOrderContext_h

#include <atomic>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/RiskDO.h"
#include "../utility/autofillmap.h"
#include "../utility/cuckoohashmap_wrapper.h"
#include "../pricingengine/IPricingDataContext.h"
#include "IUserPositionContext.h"
#include "ordermgr_export.h"

typedef cuckoohashmap_wrapper<ContractKey, UserPositionExDO_Ptr, ContractKeyHash> OTCPositionType;
typedef cuckoohashmap_wrapper<std::string, OTCPositionType> OTCPortfolioPositionType;
typedef cuckoohash_map<std::string, OTCPortfolioPositionType> OTCUserPositionType;

class ORDERMGR_CLASS_EXPORT OTCUserPositionContext
{
public:
	virtual UserPositionExDO_Ptr UpsertPosition(const std::string & userid, const UserPositionExDO & positionDO);

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userid, const TradeRecordDO_Ptr & tradeDO);

	void Clear(void);

	virtual OTCUserPositionType& AllOTCPosition();

	virtual OTCPortfolioPositionType GetPortfolioPositionsByUser(const std::string& userID);

	virtual OTCPositionType GetPositionsByUser(const std::string& userID, const std::string& portfolio);

	virtual UserPositionExDO_Ptr GetPosition(const std::string & userID, const std::string & portfolio, const ContractKey & contract);

	virtual bool RemovePosition(const std::string & userID, const std::string & portfolio, const ContractKey & contract);

	ContractMap<double> GenSpreadPoints(const PortfolioKey& portfolioKey);

	bool GetRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr, const std::string& userID, const std::string& portfolio, UnderlyingRiskMap& risks);

	virtual bool GetValuationRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
		const std::string & userID, const ValuationRiskDO& valuationRisk, UnderlyingRiskMap& risks);

protected:
	cuckoohash_map<std::string, OTCPortfolioPositionType> _userPositionMap;

private:

};

#endif