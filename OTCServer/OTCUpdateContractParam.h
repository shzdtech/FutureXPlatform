/***********************************************************************
 * Module:  OTCUpdateContractParam.h
 * Author:  milk
 * Modified: 2015年8月22日 15:50:23
 * Purpose: Declaration of the class OTCUpdateContractParam
 ***********************************************************************/

#if !defined(__OTCServer_OTCUpdateContractParam_h)
#define __OTCServer_OTCUpdateContractParam_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCUpdateContractParam : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif