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
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPAccountLogin : public LoginHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
	std::shared_ptr<UserInfoDO> Login(const dataobj_ptr reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

private:

};

#endif