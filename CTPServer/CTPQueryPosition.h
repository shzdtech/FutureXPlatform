/***********************************************************************
 * Module:  CTPQueryPosition.h
 * Author:  milk
 * Modified: 2015年7月11日 13:38:08
 * Purpose: Declaration of the class CTPQueryPosition
 ***********************************************************************/

#if !defined(__CTP_CTPQueryPosition_h)
#define __CTP_CTPQueryPosition_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryPosition : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif