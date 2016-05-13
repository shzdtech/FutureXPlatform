/***********************************************************************
 * Module:  CTPUnsubMarketData.h
 * Author:  milk
 * Modified: 2015年3月6日 20:11:00
 * Purpose: Declaration of the class CTPUnsubMarketData
 ***********************************************************************/

#if !defined(__CTP_CTPUnsubMarketData_h)
#define __CTP_CTPUnsubMarketData_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPUnsubMarketData : public MessageHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif