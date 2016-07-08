/***********************************************************************
 * Module:  CTPOTCTradingDeskProcessor.h
 * Author:  milk
 * Modified: 2015年11月6日 22:49:14
 * Purpose: Declaration of the class CTPOTCTradingDeskProcessor
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCTradingDeskProcessor_h)
#define __CTPOTC_CTPOTCTradingDeskProcessor_h

#include "../CTPServer/CTPProcessor.h"

class CTPOTCTradingDeskProcessor : public CTPProcessor
{
public:
	CTPOTCTradingDeskProcessor(const std::map<std::string, std::string>& configMap);
   ~CTPOTCTradingDeskProcessor();
   void Initialize(void);

protected:
   bool OnSessionClosing(void);

private:

};

#endif