/***********************************************************************
 * Module:  OTCQueryContractParam.h
 * Author:  milk
 * Modified: 2015年8月23日 9:48:32
 * Purpose: Declaration of the class OTCQueryContractParam
 ***********************************************************************/

#if !defined(__OTCServer_OTCQueryContract_h)
#define __OTCServer_OTCQueryContract_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCQueryContractParam : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif