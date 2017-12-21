#pragma once
#include "../dataobject/UserPositionDO.h"
#include "../dataobject/TemplateDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS PositionDAO
{
public:
	static VectorDO_Ptr<UserPositionExDO> QueryLastDayPosition(const std::string & userid, const std::string& tradingDay);
	static VectorDO_Ptr<UserPositionExDO> QueryOTCLastDayPosition(const std::string & userid, const std::string & tradingDay);
	static bool SyncPosition(const std::string & userid, const std::string& sysUserId, const std::vector<UserPositionDO>& positions);
};

