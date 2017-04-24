/***********************************************************************
 * Module:  PortfolioPositionContext..h
 * Author:  milk
 * Modified: 2015年10月28日 11:18:40
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_PortfolioPositionContext_h)
#define __ordermanager_PortfolioPositionContext_h

#include "IUserPositionContext.h"
#include "ordermgr_export.h"

typedef cuckoohash_map<std::string, cuckoohashmap_wrapper<std::string, ContractPosition>> UserPortfolioPosition;

class ORDERMGR_CLASS_EXPORT PortfolioPositionContext : public IUserPositionContext
{
public:

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO, bool updateYdPosition = false, bool closeYdFirst = false);

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userid, const TradeRecordDO_Ptr & tradeDO, PositionDirectionType pd, InstrumentDO* pContractInfo, bool closeYdFirst = false);

	void Clear(void);

	virtual UserPortfolioPosition& AllUserPortfolioPosition();

	virtual PortfolioPosition GetPortfolioPositionsByUser(const std::string& userID);

	virtual ContractPosition GetPositionsByUser(const std::string& userID, const std::string& portfolio = "");

	virtual UserPositionExDO_Ptr GetPosition(const std::string& userID, const std::string& instumentID, PositionDirectionType direction, const std::string& portfolio = "");

	virtual bool RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio = "");

private:
	UserPortfolioPosition _userPositionMap;
};

#endif