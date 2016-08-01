/***********************************************************************
 * Module:  CTPOTCSubMarketData.h
 * Author:  milk
 * Modified: 2015年8月2日 14:14:39
 * Purpose: Declaration of the class CTPOTCSubMarketData
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCSubMarketData_h)
#define __CTPOTC_CTPOTCSubMarketData_h

#include "../message/MessageHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCSubMarketData : public MessageHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
	dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif