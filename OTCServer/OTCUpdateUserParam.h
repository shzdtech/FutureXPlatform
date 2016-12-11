/***********************************************************************
 * Module:  OTCUpdateUserParam.h
 * Author:  milk
 * Modified: 2015年8月22日 15:50:10
 * Purpose: Declaration of the class OTCUpdateUserParam
 ***********************************************************************/

#if !defined(__OTCServer_OTCUpdateUserParam_h)
#define __OTCServer_OTCUpdateUserParam_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCUpdateUserParam : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
private:

};

#endif