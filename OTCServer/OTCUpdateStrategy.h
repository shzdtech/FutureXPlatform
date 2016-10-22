/***********************************************************************
 * Module:  OTCUpdateStrategy.h
 * Author:  milk
 * Modified: 2015年8月23日 18:51:51
 * Purpose: Declaration of the class OTCUpdateStrategy
 ***********************************************************************/

#if !defined(__OTCServer_OTCUpdateStrategy_h)
#define __OTCServer_OTCUpdateStrategy_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCUpdateStrategy : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif