/***********************************************************************
 * Module:  CTPOTCLogin.h
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Declaration of the class CTPOTCLogin
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCLogin_h)
#define __CTPOTC_CTPOTCLogin_h

#include "../CTPServer/CTPTDLoginSA.h"
#include "../dataobject/UserInfoDO.h"
#include "../ordermanager/OTCUserPositionContext.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCLogin : public CTPTDLoginSA
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
	void LoadOTCUserPosition(OTCUserPositionContext& positionCtx, const IUserInfo& userInfo);

private:

};

#endif