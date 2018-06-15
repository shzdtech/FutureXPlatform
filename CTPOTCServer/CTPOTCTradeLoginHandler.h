/***********************************************************************
 * Module:  CTPOTCTradeLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Declaration of the class CTPOTCTradeLoginHandler
 ***********************************************************************/

#if !defined(__CTP_CTPOTCTradeLoginHandler_h)
#define __CTP_CTPOTCTradeLoginHandler_h

#include "../CTPServer/CTPTradeLoginHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeLoginHandler : public CTPTradeLoginHandler
{
public:
	virtual CTPTradeWorkerProcessorBase * GetWorkerProcessor(const IMessageProcessor_Ptr & msgProcessor);

protected:
private:

};

#endif