/***********************************************************************
 * Module:  CTPOTCSubMarketData.h
 * Author:  milk
 * Modified: 2015年8月2日 14:14:39
 * Purpose: Declaration of the class CTPOTCSubMarketData
 ***********************************************************************/

#if !defined(__CTP_CTPOTCSubMarketData_h)
#define __CTP_CTPOTCSubMarketData_h

#include "../message/MessageHandler.h"

class CTPOTCSubMarketData : public MessageHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif