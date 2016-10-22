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
	int LoginFunction(ISession* session, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& severName);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif