/***********************************************************************
 * Module:  UserPositionContext..h
 * Author:  milk
 * Modified: 2015年10月28日 11:18:40
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_UserPositionContext_h)
#define __ordermanager_UserPositionContext_h

#include "IUserPositionContext.h"

#include "ordermgr_export.h"


class ORDERMGR_CLASS_EXPORT UserPositionContext : public IUserPositionContext
{
public:
	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO, bool updateYdPosition = false, bool closeYdFirst = false);

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userid, const TradeRecordDO_Ptr & tradeDO, PositionDirectionType pd, InstrumentDO* pContractInfo, bool closeYdFirst = false);

	void Clear(void);

	virtual cuckoohash_map<std::string, ContractPosition>& AllUserPosition();

	virtual PortfolioPosition GetPortfolioPositionsByUser(const std::string& userID);

	virtual ContractPosition GetPositionsByUser(const std::string& userID, const std::string& portfolio = "");

	virtual UserPositionExDO_Ptr GetPosition(const std::string& userID, const std::string& instumentID, PositionDirectionType direction, const std::string& portfolio = "");

	virtual bool RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio = "");

	bool FreezePosition(const OrderRequestDO & orderRequestDO, int& todayVol, int& ydVol);

private:
	cuckoohash_map<std::string, ContractPosition> _userPositionMap;
};

#endif