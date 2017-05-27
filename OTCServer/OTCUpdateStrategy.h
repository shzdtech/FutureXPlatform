/***********************************************************************
 * Module:  OTCUpdateStrategy.h
 * Author:  milk
 * Modified: 2015年8月23日 18:51:51
 * Purpose: Declaration of the class OTCUpdateStrategy
 ***********************************************************************/

#if !defined(__OTCServer_OTCUpdateStrategy_h)
#define __OTCServer_OTCUpdateStrategy_h

#include "../message/MessageHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCUpdateStrategy : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif