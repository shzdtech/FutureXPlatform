/***********************************************************************
 * Module:  AdminLoginHandler.h
 * Author:  milk
 * Modified: 2016年5月14日 22:10:51
 * Purpose: Declaration of the class AdminLoginHandler
 ***********************************************************************/

#if !defined(__AdminServer_AdminLoginHandler_h)
#define __AdminServer_AdminLoginHandler_h

#include "../message/NopHandler.h"
#include "AdminServerExport.h"

class ADMINSERVER_CLASS_EXPORT AdminLoginHandler : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif