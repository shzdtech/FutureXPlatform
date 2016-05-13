/***********************************************************************
 * Module:  CTPOTCNewUser.h
 * Author:  milk
 * Modified: 2016年5月12日 17:33:04
 * Purpose: Declaration of the class CTPOTCNewUser
 ***********************************************************************/

#if !defined(__CTPOTCServer_CTPOTCNewUser_h)
#define __CTPOTCServer_CTPOTCNewUser_h

#include "../message/NopHandler.h"

class CTPOTCNewUser : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif