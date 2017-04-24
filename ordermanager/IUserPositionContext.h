#pragma once

#include "../dataobject/UserPositionDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "../utility/cuckoohashmap_wrapper.h"
#include "../utility/pairhash.h"
#include "../common/typedefs.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TradeRecordDO.h"

typedef cuckoohashmap_wrapper<std::pair<std::string, int>, UserPositionExDO_Ptr, pairhash<std::string, int>> ContractPosition;
typedef cuckoohashmap_wrapper<std::string, ContractPosition> PortfolioPosition;

class IUserPositionContext
{
public:
	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userId, const UserPositionExDO& positionDO, bool updateYdPosition = false, bool closeYdFirst = false) = 0;

	virtual UserPositionExDO_Ptr UpsertPosition(const std::string& userid, const TradeRecordDO_Ptr & tradeDO, PositionDirectionType pd, InstrumentDO* pContractInfo, bool closeYdFirst = false) = 0;

	virtual void Clear(void) = 0;

	virtual PortfolioPosition GetPortfolioPositionsByUser(const std::string& userID) = 0;

	virtual ContractPosition GetPositionsByUser(const std::string& userID, const std::string& portfolio = "") = 0;

	virtual UserPositionExDO_Ptr GetPosition(const std::string& userID, const std::string& instumentID, PositionDirectionType direction, const std::string& portfolio = "") = 0;

	virtual bool RemovePosition(const std::string & userID, const std::string & instumentID, PositionDirectionType direction, const std::string& portfolio = "") = 0;
};