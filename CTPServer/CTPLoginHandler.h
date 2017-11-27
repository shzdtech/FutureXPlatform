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
#include "../bizutility/ExchangeRouterTable.h"
#include "../dataobject/UserInfoDO.h"
#include "CTPAPISwitch.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPLoginHandler : public LoginHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	virtual std::shared_ptr<UserInfoDO> LoginFromServer(const IMessageProcessor_Ptr& msgProcessor, const std::shared_ptr<UserInfoDO>& userInfo_Ptr,
		uint requestId, const std::string& serverName = "") = 0;

protected:
private:

};

#endif