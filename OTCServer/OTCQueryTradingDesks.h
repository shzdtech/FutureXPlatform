/***********************************************************************
 * Module:  OTCTradingDesks.h
 * Author:  milk
 * Modified: 2015年8月27日 20:23:20
 * Purpose: Declaration of the class OTCTradingDesks
 ***********************************************************************/

#if !defined(__OTCServer_QueryTradingDesks_h)
#define __OTCServer_QueryTradingDesks_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCQueryTradingDesks : public NopHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif