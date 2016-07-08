/***********************************************************************
 * Module:  CTPAccountLogin.h
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Declaration of the class CTPAccountLogin
 ***********************************************************************/

#if !defined(__CTP_CTPAccountLogin_h)
#define __CTP_CTPAccountLogin_h

#include "../message/LoginHandler.h"
#include "../dataobject/UserInfoDO.h"

class CTPAccountLogin : public LoginHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
	std::shared_ptr<UserInfoDO> Login(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);

private:

};

#endif