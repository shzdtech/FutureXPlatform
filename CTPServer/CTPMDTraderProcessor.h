/***********************************************************************
 * Module:  CTPMDTraderProcessor.h
 * Author:  milk
 * Modified: 2015年10月24日 13:47:15
 * Purpose: Declaration of the class CTPMDTraderProcessor
 ***********************************************************************/

#if !defined(__CTP_CTPMDTraderProcessor_h)
#define __CTP_CTPMDTraderProcessor_h

#include "CTPProcessor.h"
#include "CTPMarketDataProcessor.h"
#include "CTPTradeProcessor.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPMDTraderProcessor : public CTPProcessor
{
public:
	CTPMDTraderProcessor(
		const std::map<std::string, std::string>& configMap, 
		std::shared_ptr<CTPMarketDataProcessor> mdProc, 
		std::shared_ptr<CTPTradeProcessor> tradeProc);
	~CTPMDTraderProcessor();

protected:
	void OnInit(void);

private:
	std::shared_ptr<CTPMarketDataProcessor> _mdproc;
	std::shared_ptr<CTPTradeProcessor> _traderproc;
};

#endif