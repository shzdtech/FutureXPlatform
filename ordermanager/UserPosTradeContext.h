/***********************************************************************
 * Module:  UserPosTradeContext..h
 * Author:  milk
 * Modified: 2016年11月06日 11:18:40
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_UserPosTradeContext_h)
#define __ordermanager_UserPosTradeContext_h

#include <atomic>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TradeRecordDO.h"
#include <libcuckoo/cuckoohash_map.hh>
#include "UserTradeContext.h"
#include "../utility/cuckoohashmap_wrapper.h"
#include "ordermgr_export.h"

class ORDERMGR_CLASS_EXPORT UserPosTradeContext : public UserTradeContext
{
public:


private:

};

#endif