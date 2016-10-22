/***********************************************************************
 * Module:  TestingLoginHandler.h
 * Author:  milk
 * Modified: 2016年4月29日 19:24:33
 * Purpose: Declaration of the class TestingLoginHandler
 ***********************************************************************/

#if !defined(__TestingServer_TestingLoginHandler_h)
#define __TestingServer_TestingLoginHandler_h

#include "../message/NopHandler.h"

class TestingLoginHandler : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif