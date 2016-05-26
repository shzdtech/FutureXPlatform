/***********************************************************************
 * Module:  CTPTradeLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Declaration of the class CTPTradeLoginHandler
 ***********************************************************************/

#if !defined(__CTP_CTPTradeLoginHandler_h)
#define __CTP_CTPTradeLoginHandler_h

#include "CTPLoginHandler.h"

class CTPTradeLoginHandler : public CTPLoginHandler
{
public:
	int LoginFunction(IRawAPI* rawAPI, CThostFtdcReqUserLoginField* loginInfo, uint requestId);

protected:
private:

};

#endif