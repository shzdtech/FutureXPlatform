/***********************************************************************
 * Module:  CTPOTCTradeLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Declaration of the class CTPOTCTradeLoginHandler
 ***********************************************************************/

#if !defined(__CTP_CTPOTCTradeLoginHandler_h)
#define __CTP_CTPOTCTradeLoginHandler_h

#include "../CTPServer/CTPTradeLoginHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCTradeLoginHandler : public CTPTradeLoginHandler
{
public:
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif