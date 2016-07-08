/***********************************************************************
 * Module:  CTPOTCLogin.h
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Declaration of the class CTPOTCLogin
 ***********************************************************************/

#if !defined(__CTP_CTPOTCLogin_h)
#define __CTP_CTPOTCLogin_h

#include "CTPAccountLogin.h"
#include "../dataobject/UserInfoDO.h"

class CTPOTCLogin : public CTPAccountLogin
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);

protected:

private:

};

#endif