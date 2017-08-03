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
	virtual bool CreateCTPAPI(const std::string& flowId, const std::string& serverAddr = "");
	CTPRawAPI_Ptr& RawAPI_Ptr(void);

	enum DataLoadType
	{
		NO_DATA_LOADED = 0,
		ORDER_DATA_LOADED = 0x1,
		TRADE_DATA_LOADED = 0x2,
		POSITION_DATA_LOADED = 0x4,
		ACCOUNT_DATA_LOADED = 0x8,
		EXCHANGE_DATA_LOADED = 0x10,
		INSTRUMENT_DATA_LOADED = 0x20,
		PORTFOLIO_DATA_LOADED = 0x40,
		ALL_DATA_LOADED = 0xFFFFFFFF,
	};

	volatile int DataLoadMask;

public:
	uint32_t LoginSerialId;
	static std::chrono::seconds DefaultQueryTime;
	static std::string FlowPath;

protected:
	CTPRawAPI_Ptr _rawAPI;	

private:
};

#endif