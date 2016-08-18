#if !defined(__databaseop_TradeDAO_h)
#define __databaseop_TradeDAO_h

#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TemplateDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS TradeDAO
{
public:
	static VectorDO_Ptr<TradeRecordDO> QueryTrade(const std::string& userid, const ContractKey & contractKey,
		const std::string& startDt, const std::string& endDt);
	
};

#endif