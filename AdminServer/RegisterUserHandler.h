/***********************************************************************
 * Module:  RegisterUserHandler.h
 * Author:  milk
 * Modified: 2016年5月14日 22:58:35
 * Purpose: Declaration of the class RegisterUserHandler
 ***********************************************************************/

#if !defined(__AdminServer_RegisterUserHandler_h)
#define __AdminServer_RegisterUserHandler_h

#include "../message/NopHandler.h"


class RegisterUserHandler : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif