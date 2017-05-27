/***********************************************************************
 * Module:  PortfolioPositionContext..h
 * Author:  milk
 * Modified: 2015年10月28日 11:18:40
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_PortfolioPositionContext_h)
#define __ordermanager_PortfolioPositionContext_h

#include "IUserPositionContext.h"
#include "../pricingengine/IPricingDataContext.h"

#include "ordermgr_export.h"

typedef cuckoohash_map<std::string, cuckoohashmap_wrapper<std::string, ContractPosition>> UserPortfolioPosition;

class ORDERMGR_CLASS_EXPORT PortfolioPositionContext : public IUserPositionContext
{
public:
	PortfolioPositionContext(const IPricingDataContext_Ptr& pricingCtx);

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO, bool updateYdPosition = false, bool closeYdFirst = false);

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userid, const TradeRecordDO_Ptr & tradeDO, int multiplier = 1, bool closeYdFirst = false);

	void Clear(void);

	virtual UserPortfolioPosition& AllUserPortfolioPosition();

	virtual PortfolioPosition GetPortfolioPositionsByUser(const std::string& userID);

	virtual ContractPosition GetPositionsByUser(const std::string& userID, const std::string& portfolio = "");

	virtual UserPositionExDO_Ptr GetPosition(const std::string& userID, const std::string& instumentID, PositionDirectionType direction, const std::string& portfolio = "");

	virtual bool RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio = "");

	virtual bool AllPosition(std::vector<UserPositionExDO_Ptr>& positions);

	virtual bool GetRiskByPortfolio(const std::string& userID, const std::string& portfolio, UnderlyingRiskMap& risks);

private:
	UserPortfolioPosition _userPositionMap;
	IPricingDataContext_Ptr _pricingCtx;
};

#endif