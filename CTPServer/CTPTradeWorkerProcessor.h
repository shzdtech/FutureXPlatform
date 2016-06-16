/***********************************************************************
 * Module:  CTPTradeWorkerProcessor.h
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Declaration of the class CTPTradeWorkerProcessor
 ***********************************************************************/

#if !defined(__CTPServer_CTPTradeWorkerProcessor_h)
#define __CTPServer_CTPTradeWorkerProcessor_h

#include "CTPTradeProcessor.h"
#include <future>
#include "../message/UserInfo.h"


#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerProcessor : public CTPTradeProcessor
{
public:
   CTPTradeWorkerProcessor(const std::map<std::string, std::string>& configMap);
   ~CTPTradeWorkerProcessor();
   void Initialize(void);
   int LoginDefault(void);
   int LoginIfNeed(void);
   
   int RetryInterval = 60000;
   bool InstrmentsLoaded;

protected:
   UserInfo _defaultUser;
   std::future<void> _initializer;

public:
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};

#endif