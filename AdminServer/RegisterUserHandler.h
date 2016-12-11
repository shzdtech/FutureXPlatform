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
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif