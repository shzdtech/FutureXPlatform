/***********************************************************************
 * Module:  TestingReturnMarketDataHandler.h
 * Author:  milk
 * Modified: 2016年4月29日 23:13:22
 * Purpose: Declaration of the class TestingReturnMarketDataHandler
 ***********************************************************************/

#if !defined(__TestingServer_TestingReturnMarketDataHandler_h)
#define __TestingServer_TestingReturnMarketDataHandler_h

#include "../message/NopHandler.h"

class TestingReturnMarketDataHandler : public NopHandler
{
public:
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif