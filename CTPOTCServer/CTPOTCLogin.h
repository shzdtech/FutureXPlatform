/***********************************************************************
 * Module:  CTPOTCLogin.h
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Declaration of the class CTPOTCLogin
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCLogin_h)
#define __CTPOTC_CTPOTCLogin_h

#include "../CTPServer/CTPAccountLogin.h"
#include "../dataobject/UserInfoDO.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCLogin : public CTPAccountLogin
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:

private:

};

#endif