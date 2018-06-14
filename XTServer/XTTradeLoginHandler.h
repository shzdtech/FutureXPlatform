/***********************************************************************
 * Module:  CTPTradeLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Declaration of the class CTPTradeLoginHandler
 ***********************************************************************/

#if !defined(__XT_CTPTradeLoginHandler_h)
#define __XT_CTPTradeLoginHandler_h

#include "CTPLoginHandler.h"
#include "XTTradeWorkerProcessor.h"
#include "xt_export.h"

class XT_CLASS_EXPORT CTPTradeLoginHandler : public CTPLoginHandler
{
public:
	virtual std::shared_ptr<UserInfoDO> LoginFromServer(const XTProcessor_Ptr& msgProcessor,
		const std::shared_ptr<UserInfoDO> & userInfoDO_Ptr, uint requestId, const std::string& serverName);
	virtual CTPTradeWorkerProcessor * GetWorkerProcessor(const IMessageProcessor_Ptr & msgProcessor);
	virtual bool LoginFromDB(const IMessageProcessor_Ptr& msgProcessor);
	virtual void LoadUserRiskModel(const IMessageProcessor_Ptr& msgProcessor);

	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif