/***********************************************************************
 * Module:  OTCQueryStrategy.h
 * Author:  milk
 * Modified: 2015年8月22日 15:46:02
 * Purpose: Declaration of the class OTCQueryStrategy
 ***********************************************************************/

#if !defined(__OTCServer_OTCQueryStrategy_h)
#define __OTCServer_OTCQueryStrategy_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCQueryStrategy : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif