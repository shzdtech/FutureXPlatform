/***********************************************************************
 * Module:  XTTradeLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Declaration of the class XTTradeLoginHandler
 ***********************************************************************/

#if !defined(__XT_XTTradeLoginHandler_h)
#define __XT_XTTradeLoginHandler_h

#include "../CTPServer/CTPProcessor.h"
#include "../CTPServer/CTPTradeLoginHandler.h"
#include "XTTradeWorkerProcessor.h"
#include "xt_export.h"

class XT_CLASS_EXPORT XTTradeLoginHandler : public CTPTradeLoginHandler
{
public:
	virtual std::shared_ptr<UserInfoDO> LoginFromServer(const CTPProcessor_Ptr& msgProcessor,
		const std::shared_ptr<UserInfoDO> & userInfoDO_Ptr, uint requestId, const std::string& serverName);

	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif