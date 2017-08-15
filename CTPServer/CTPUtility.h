/***********************************************************************
 * Module:  CTPUtility.h
 * Author:  milk
 * Modified: 2015年7月16日 21:32:43
 * Purpose: Declaration of the class CTPUtility
 ***********************************************************************/

#if !defined(__CTP_CTPUtility_h)
#define __CTP_CTPUtility_h

#include "../message/ISession.h"
#include "../message/BizError.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/BankOpResultDO.h"
#include "../dataobject/UserAccountRegisterDO.h"
#include "../dataobject/UserPositionDO.h"
#include "CTPAPISwitch.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPUtility
{
public:
	static void CheckNotFound(const void* pRspData);
	static bool HasError(CThostFtdcRspInfoField* pRspInfo);
	static void CheckError(const void* pRspInfo);
	static std::shared_ptr<ApiException> HasReturnError(const int rtnCode);
	static void CheckReturnError(const int rtncode);
	static bool IsOrderActive(TThostFtdcOrderStatusType status);
	static OrderStatusType CheckOrderStatus(TThostFtdcOrderStatusType status, TThostFtdcOrderSubmitStatusType submitStatus);
	static void LogFrontDisconnected(int nReseason, std::string& errMsg);

	static OrderDO_Ptr ParseRawOrder(CThostFtdcInputOrderField *pOrder, CThostFtdcRspInfoField *pRsp, int sessionID, OrderDO_Ptr baseOrder = nullptr);
	static OrderDO_Ptr ParseRawOrder(CThostFtdcOrderField *pOrder, OrderDO_Ptr baseOrder = nullptr);
	static OrderDO_Ptr ParseRawOrder(CThostFtdcInputOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRsp, OrderDO_Ptr baseOrder = nullptr);
	static OrderDO_Ptr ParseRawOrder(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRsp, OrderDO_Ptr baseOrder = nullptr);

	static TradeRecordDO_Ptr ParseRawTrade(CThostFtdcTradeField *pTrade);

	static BankOpResultDO_Ptr ParseRawTransfer(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRsp = nullptr);
	static BankOpResultDO_Ptr ParseRawTransfer(CThostFtdcRspTransferField *pRspTransfer, CThostFtdcRspInfoField *pRsp = nullptr);
	static BankOpResultDO_Ptr ParseRawTransfer(CThostFtdcTransferSerialField *pRspTransfer);

	static UserPositionExDO_Ptr ParseRawPosition(CThostFtdcInvestorPositionField* pRspPosition);

	static UserAccountRegisterDO_Ptr ParseUserBankAccout(CThostFtdcAccountregisterField *pAccount, CThostFtdcRspInfoField *pRsp = nullptr);
	static UserAccountRegisterDO_Ptr ParseUserBankAccout(CThostFtdcReqQueryAccountField *pAccount, CThostFtdcRspInfoField *pRsp = nullptr);
	static UserAccountRegisterDO_Ptr ParseUserBankAccout(CThostFtdcNotifyQueryAccountField *pAccount);

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