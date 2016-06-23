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
	int LoginFunction(IRawAPI* rawAPI, ISession* session, 
		CThostFtdcReqUserLoginField* loginInfo, uint requestId);
	dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif