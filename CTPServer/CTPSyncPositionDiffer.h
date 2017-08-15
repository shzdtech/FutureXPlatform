/***********************************************************************
 * Module:  CTPSyncPositionDiffer.h
 * Author:  milk
 * Modified: 2015年7月11日 13:38:54
 * Purpose: Declaration of the class CTPSyncPositionDiffer
 ***********************************************************************/

#if !defined(__CTP_CTPSyncPositionDiffer_h)
#define __CTP_CTPSyncPositionDiffer_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPSyncPositionDiffer : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif