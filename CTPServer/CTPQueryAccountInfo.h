/***********************************************************************
 * Module:  CTPQueryAccountInfo.h
 * Author:  milk
 * Modified: 2015年7月11日 13:37:12
 * Purpose: Declaration of the class CTPQueryAccountInfo
 ***********************************************************************/

#if !defined(__CTP_CTPQueryAccountInfo_h)
#define __CTP_CTPQueryAccountInfo_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryAccountInfo : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif