/***********************************************************************
 * Module:  CTPOCTUnSubMarketData.h
 * Author:  milk
 * Modified: 2016年4月19日 12:59:04
 * Purpose: Declaration of the class CTPOCTUnSubMarketData
 ***********************************************************************/

#if !defined(__CTPOTCServer_CTPOCTUnSubMarketData_h)
#define __CTPOTCServer_CTPOCTUnSubMarketData_h

#include "../message/MessageHandler.h"

class CTPOCTUnSubMarketData : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector rawParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif