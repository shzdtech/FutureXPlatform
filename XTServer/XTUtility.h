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
#include "../dataobject/AccountInfoDO.h"
#include "../dataobject/UserPositionDO.h"
#include "include/XtDataType.h"
#include "XTRawAPI.h"
#include "xt_export.h"

using namespace xti;

class XT_CLASS_EXPORT XTUtility
{
public:
	static void CheckTradeInit(XTRawAPI* pCtpAPI);
	static bool HasTradeInit(XTRawAPI * pCtpAPI);
	static void CheckNotFound(const void* pRspData);
	static bool HasError(const XtError* pRspInfo);
	static void CheckError(const void* pRspInfo);
	static std::shared_ptr<ApiException> HasReturnError(const int rtnCode);
	static void CheckReturnError(const int rtncode);
	
	static AccountInfoDO_Ptr ParseRawAccountInfo(const CAccountDetail* pAccountInfo);

	static EOperationType GetOperationType(ProductType productType, DirectionType direction, OrderOpenCloseType openClose);
	static bool IsOrderActive(EEntrustStatus status);
	static OrderStatusType CheckOrderStatus(EEntrustStatus status, EEntrustSubmitStatus submitStatus);

	static OrderDO_Ptr ParseRawOrder(int requestId, int orderId);
	static OrderDO_Ptr ParseRawOrder(const COrderDetail *pOrder, const XtError *pRsp, int sessionID, OrderDO_Ptr baseOrder = nullptr);
	static void ParseRawOrder(OrderDO_Ptr& baseOrder, const XtError *pRsp, int sessionID);

	static TradeRecordDO_Ptr ParseRawTrade(const CDealDetail *pTrade);

	static UserPositionExDO_Ptr ParseRawPosition(const CPositionDetail* pRspPosition, const std::string& userId);

	static uint32_t ParseOrderMessageID(OrderStatusType orderStatus);

	static std::string MakeUserID(const std::string& brokerID, const std::string& investorID);

	static inline uint64_t ToUInt64(const char* str) { return std::strtoull(str, nullptr, 10); }
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