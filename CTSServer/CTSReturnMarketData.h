/***********************************************************************
 * Module:  CTSReturnMarketData.h
 * Author:  milk
 * Modified: 2015年11月8日 22:26:40
 * Purpose: Declaration of the class CTSReturnMarketData
 ***********************************************************************/

#if !defined(__CTSServer_CTSReturnMarketData_h)
#define __CTSServer_CTSReturnMarketData_h

#include "../message/IMessageHandler.h"

class CTSReturnMarketData : public IMessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif