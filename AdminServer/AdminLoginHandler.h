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
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif