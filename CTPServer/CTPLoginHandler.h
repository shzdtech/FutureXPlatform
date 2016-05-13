/***********************************************************************
 * Module:  CTPLoginHandler.h
 * Author:  milk
 * Modified: 2014年10月6日 21:43:46
 * Purpose: Declaration of the class CTPLoginHandler
 ***********************************************************************/

#if !defined(__CTP_CTPLoginHandler_h)
#define __CTP_CTPLoginHandler_h

#include "../message/LoginHandler.h"
#include "../common/typedefs.h"
#include "tradeapi/ThostFtdcUserApiStruct.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPLoginHandler : public LoginHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);
	virtual int LoginFunction(IRawAPI* rawAPI, CThostFtdcReqUserLoginField* loginInfo) = 0;

protected:
private:

};

#endif