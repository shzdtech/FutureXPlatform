/***********************************************************************
 * Module:  CTPOTCSessionProcessor.h
 * Author:  milk
 * Modified: 2015年11月6日 22:49:14
 * Purpose: Declaration of the class CTPOTCSessionProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradingDeskProcessor_h)
#define __CTPOTC_CTPOTCTradingDeskProcessor_h

#include "../CTPServer/CTPProcessor.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCSessionProcessor : public CTPProcessor
{
public:
	CTPOTCSessionProcessor();
   ~CTPOTCSessionProcessor();
   void Initialize(IServerContext* serverCtx);

protected:
   bool OnSessionClosing(void);

private:

};

#endif