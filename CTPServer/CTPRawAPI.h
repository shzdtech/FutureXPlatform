/***********************************************************************
 * Module:  CTPRawAPI.h
 * Author:  milk
 * Modified: 2015年9月5日 9:41:36
 * Purpose: Declaration of the class CTPRawAPI
 ***********************************************************************/

#if !defined(__CTP_CTPRawAPI_h)
#define __CTP_CTPRawAPI_h

#include "../message/IRawAPI.h"
#include "tradeapi/ThostFtdcMdApi.h"
#include "tradeapi/ThostFtdcTraderApi.h"

class CTPRawAPI : public IRawAPI
{
public:
	CTPRawAPI(){ MdAPI = nullptr; TrdAPI = nullptr; }

	CThostFtdcMdApi* MdAPI;
	CThostFtdcTraderApi* TrdAPI;

protected:
private:

};

#endif