/***********************************************************************
 * Module:  CTPOTCLogin.h
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Declaration of the class CTPOTCLogin
 ***********************************************************************/

#if !defined(__CTP_CTPOTCLogin_h)
#define __CTP_CTPOTCLogin_h

#include "../message/LoginHandler.h"
#include "../dataobject/UserInfoDO.h"

class CTPOTCLogin : public LoginHandler
{
public:
	dataobj_ptr HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
	std::shared_ptr<UserInfoDO> GenUserInfoDO(IUserInfo* pUserInfo);

private:

};

#endif