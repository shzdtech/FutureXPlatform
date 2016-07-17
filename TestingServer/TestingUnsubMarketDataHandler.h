/***********************************************************************
 * Module:  TestingUnsubMarketDataHandler.h
 * Author:  milk
 * Modified: 2016年4月29日 19:25:47
 * Purpose: Declaration of the class TestingUnsubMarketDataHandler
 ***********************************************************************/

#if !defined(__TestingServer_TestingUnsubMarketDataHandler_h)
#define __TestingServer_TestingUnsubMarketDataHandler_h

#include "../message/NopHandler.h"

class TestingUnsubMarketDataHandler : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif