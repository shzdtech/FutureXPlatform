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
#include "tradeapi/ThostFtdcUserApiStruct.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPUtility
{
public:
	static bool HasError(const void* pRspInfo);
	static void CheckError(const void* pRspInfo);
	static void CheckLogin(const ISession* session);
	static BizError_Ptr HasReturnError(const int rtnCode);
	static void CheckReturnError(const int rtncode);
	static bool IsOrderActive(const int status);
	static OrderStatus CheckOrderStatus(const int status, const int submitStatus);
	static OrderDO_Ptr ParseRawOrder(CThostFtdcOrderField *pOrder);
	static OrderDO_Ptr ParseRawOrderAction(CThostFtdcInputOrderActionField *pOrder, CThostFtdcRspInfoField *pRsp, OrderStatus orderstatus);
	static OrderDO_Ptr ParseRawOrderInput(CThostFtdcInputOrderField *pOrder, CThostFtdcRspInfoField *pRsp, OrderStatus orderstatus);

protected:
private:

};

static const int NULL_RET = -1;

#endif