/***********************************************************************
 * Module:  CTPOTCTradeLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Declaration of the class CTPOTCTradeLoginHandler
 ***********************************************************************/

#if !defined(__XT_CTPOTCTradeLoginHandler_h)
#define __XT_CTPOTCTradeLoginHandler_h

#include "../CTPServer/CTPTradeLoginHandler.h"
#include "xt_export.h"

class XT_CLASS_EXPORT CTPOTCTradeLoginHandler : public CTPTradeLoginHandler
{
public:
	CTPTradeWorkerProcessor * GetWorkerProcessor(const IMessageProcessor_Ptr & msgProcessor);

protected:
private:

};

#endif