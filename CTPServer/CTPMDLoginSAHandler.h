/***********************************************************************
 * Module:  CTPMDLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:15
 * Purpose: Declaration of the class CTPMDLoginHandler
 ***********************************************************************/

#if !defined(__CTP_CTPMDLoginSAHandler_h)
#define __CTP_CTPMDLoginSAHandler_h

#include "../message/LoginHandler.h"
#include "../dataobject/UserInfoDO.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPMDLoginSAHandler : public LoginHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
	std::shared_ptr<UserInfoDO> Login(const dataobj_ptr reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
};

#endif