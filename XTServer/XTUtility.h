/***********************************************************************
 * Module:  XTUtility.h
 * Author:  milk
 * Modified: 2015年7月16日 21:32:43
 * Purpose: Declaration of the class XTUtility
 ***********************************************************************/

#if !defined(__XT_XTUtility_h)
#define __XT_XTUtility_h

#include "../message/ISession.h"
#include "../message/BizError.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/BankOpResultDO.h"
#include "../dataobject/UserAccountRegisterDO.h"
#include "../dataobject/UserPositionDO.h"
#include "XTRawAPI.h"
#include "xt_export.h"

class XT_CLASS_EXPORT XTUtility
{
public:
	static void CheckTradeInit(XTRawAPI* pCtpAPI);
	static bool HasTradeInit(XTRawAPI * pCtpAPI);
	static void CheckNotFound(const void* pRspData);
	static bool HasError(XtError* pRspInfo);
	static void CheckError(const void* pRspInfo);
	static std::shared_ptr<ApiException> HasReturnError(const int rtnCode);
	static void CheckReturnError(const int rtncode);
	static bool IsOrderActive(EEntrustStatus status);
	static OrderStatusType CheckOrderStatus(EEntrustStatus status, EEntrustSubmitStatus submitStatus);

	static OrderDO_Ptr ParseRawOrder(COrderDetail *pOrder, XtError *pRsp, int sessionID, OrderDO_Ptr baseOrder = nullptr);
	static void ParseRawOrder(OrderDO_Ptr& baseOrder, XtError *pRsp, int sessionID);

	static TradeRecordDO_Ptr ParseRawTrade(CDealDetail *pTrade);

	static UserPositionExDO_Ptr ParseRawPosition(CPositionDetail* pRspPosition, const std::string& userId);

	static uint32_t ParseOrderMessageID(OrderStatusType orderStatus);

	static std::string MakeUserID(const std::string& brokerID, const std::string& investorID);

	static inline uint64_t ToUInt64(char* str) { return std::strtoull(str, nullptr, 10); }
	static inline uint64_t ToUInt64(uint32_t low32, uint32_t high32) 
	{ 
		uint64_t ret = low32;
		return ret | ((uint64_t)high32) << 32;
	}


protected:
private:

};

static const int NULL_RET = -1;

#endif