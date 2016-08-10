/***********************************************************************
 * Module:  OTCSubMarketData.h
 * Author:  milk
 * Modified: 2015年8月2日 14:14:39
 * Purpose: Declaration of the class OTCSubMarketData
 ***********************************************************************/

#if !defined(__OTCServer_OTCSubMarketData_h)
#define __OTCServer_OTCSubMarketData_h

#include "../message/NopHandler.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT OTCSubMarketData : public NopHandler
{
public:
	dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif