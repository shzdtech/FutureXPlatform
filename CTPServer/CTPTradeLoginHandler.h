/***********************************************************************
 * Module:  CTPTradeLoginHandler.h
 * Author:  milk
 * Modified: 2015年3月8日 13:12:23
 * Purpose: Declaration of the class CTPTradeLoginHandler
 ***********************************************************************/

#if !defined(__CTP_CTPTradeLoginHandler_h)
#define __CTP_CTPTradeLoginHandler_h

#include "CTPLoginHandler.h"
#include "CTPTradeWorkerProcessor.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeLoginHandler : public CTPLoginHandler
{
public:
	virtual std::shared_ptr<UserInfoDO> LoginFromServer(const IMessageProcessor_Ptr& msgProcessor,
		const std::shared_ptr<UserInfoDO> & userInfoDO_Ptr, uint requestId, const std::string& serverName);
	virtual CTPTradeProcessor * GetTradeProcessor(const IMessageProcessor_Ptr & msgProcessor);
	virtual CTPTradeWorkerProcessor * GetWorkerProcessor(const IMessageProcessor_Ptr & msgProcessor);
	virtual bool LoginFromDB(const IMessageProcessor_Ptr& msgProcessor);
	virtual void LoadUserRiskModel(const IMessageProcessor_Ptr& msgProcessor);

	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif