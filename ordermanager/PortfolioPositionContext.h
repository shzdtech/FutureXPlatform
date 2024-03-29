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


class ORDERMGR_CLASS_EXPORT PortfolioPositionContext : public IUserPositionContext
{
public:
	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO,
		bool updateYdPosition = false, bool closeYdFirst = false, bool forceSync = false);

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userid, const TradeRecordDO_Ptr & tradeDO, int multiplier = 1, bool closeYdFirst = false);

	virtual int UpdatePnL(const std::string& userID, const MarketDataDO& mdDO);

	void Clear(void);

	virtual PortfolioPositionPnL GetPortfolioPositionsPnLByUser(const std::string& userID);

	virtual ContractPositionPnL GetPositionsPnLByUser(const std::string& userID, const std::string& portfolio = "");

	virtual PositionPnLDO_Ptr GetPositionPnL(const std::string& userID, const std::string& instumentID, const std::string& portfolio = "");

	virtual UserPortfolioPosition& AllUserPortfolioPosition();

	virtual PortfolioPosition GetPortfolioPositionsByUser(const std::string& userID);

	virtual ContractPosition GetPositionsByUser(const std::string& userID, const std::string& portfolio = "");

	virtual UserPositionExDO_Ptr GetPosition(const std::string& userID, const std::string& instumentID, PositionDirectionType direction, const std::string& portfolio = "");

	virtual bool RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio = "");

	virtual bool RemoveUserPosition(const std::string & userID);

	virtual bool AllPosition(std::vector<UserPositionExDO_Ptr>& positions);

	virtual bool GetRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx, const std::string& userID, const std::string& portfolio, UnderlyingRiskMap& risks);

	virtual bool GetValuationRiskByPortfolio(const IPricingDataContext_Ptr& pricingCtx_Ptr,
		const std::string & userID, const ValuationRiskDO& valuationRisk, UnderlyingRiskMap& risks);

	virtual bool ContainsTrade(uint128 tradeID);

private:
	UserPortfolioPosition _userPositionMap;
	UserPortfolioPositionPnL _userPositionPnLMap;
	PositionTradeIDMapType _positionTradeIDMap;

};

#endif