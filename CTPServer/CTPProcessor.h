/***********************************************************************
 * Module:  CTPProcessor.h
 * Author:  milk
 * Modified: 2015年3月8日 11:34:51
 * Purpose: Declaration of the class CTPProcessor
 ***********************************************************************/

#if !defined(__CTP_CTPProcessor_h)
#define __CTP_CTPProcessor_h

#include "CTPRawAPI.h"
#include "../message/TemplateMessageProcessor.h"
#include "../message/IMessageSession.h"
#include "tradeapi/ThostFtdcMdApi.h"
#include "tradeapi/ThostFtdcTraderApi.h"
#include <chrono>

#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPProcessor : public TemplateMessageProcessor
{
public:
	CTPProcessor();
	CTPProcessor(const CTPRawAPI_Ptr& rawAPI);
	virtual IRawAPI* getRawAPI(void);
	virtual void Initialize(IServerContext* serverCtx);
	virtual int InitializeServer(const std::string& flowId, const std::string& serverAddr = "");
	virtual bool HasLogged(void);
	virtual bool ConnectedToServer(void);
	CTPRawAPI_Ptr& RawAPI_Ptr(void);

public:
	uint32_t LoginSerialId;
	static std::chrono::seconds DefaultQueryTime;
	static std::string FlowPath;

protected:
	bool _isLogged;
	bool _isConnected;
	uint32_t _tradingDay;

	CTPRawAPI_Ptr _rawAPI;	

private:
};

#endif