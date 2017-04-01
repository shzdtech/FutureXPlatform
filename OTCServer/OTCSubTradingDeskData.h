/***********************************************************************
 * Module:  OTCSubTradingDeskData.h
 * Author:  milk
 * Modified: 2015年8月2日 14:14:39
 * Purpose: Declaration of the class OTCSubTradingDeskData
 ***********************************************************************/

#if !defined(__OTCServer_OTCSubTradingDeskData_h)
#define __OTCServer_OTCSubTradingDeskData_h

#include "../message/MessageHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCSubTradingDeskData : public MessageHandler
{
public:
	dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif