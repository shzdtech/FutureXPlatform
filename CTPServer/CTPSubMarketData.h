/***********************************************************************
 * Module:  CTPSubMarketData.h
 * Author:  milk
 * Modified: 2015年3月6日 20:11:00
 * Purpose: Declaration of the class CTPSubMarketData
 ***********************************************************************/

#if !defined(__CTP_CTPSubMarketData_h)
#define __CTP_CTPSubMarketData_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPSubMarketData : public MessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif