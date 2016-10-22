/***********************************************************************
 * Module:  TestingSubMarketDataHandler.h
 * Author:  milk
 * Modified: 2016年4月29日 19:24:42
 * Purpose: Declaration of the class TestingSubMarketDataHandler
 ***********************************************************************/

#if !defined(__TestingServer_TestingSubMarketDataHandler_h)
#define __TestingServer_TestingSubMarketDataHandler_h

#include "../message/NopHandler.h"

class TestingSubMarketDataHandler : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif