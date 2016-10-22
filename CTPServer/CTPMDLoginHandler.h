/***********************************************************************
 * Module:  CTPMDLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:15
 * Purpose: Declaration of the class CTPMDLoginHandler
 ***********************************************************************/

#if !defined(__CTP_CTPMDLoginHandler_h)
#define __CTP_CTPMDLoginHandler_h

#include "CTPLoginHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPMDLoginHandler : public CTPLoginHandler
{
public:
	int LoginFunction(ISession* session, CThostFtdcReqUserLoginField* loginInfo, uint requestId, const std::string& severName);

protected:
private:

};

#endif