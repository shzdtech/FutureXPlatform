/***********************************************************************
 * Module:  CTPOTCQueryStrategy.h
 * Author:  milk
 * Modified: 2015年8月22日 15:46:02
 * Purpose: Declaration of the class CTPOTCQueryStrategy
 ***********************************************************************/

#if !defined(__CTP_CTPOTCQueryStrategy_h)
#define __CTP_CTPOTCQueryStrategy_h

#include "../message/MessageHandler.h"

class CTPOTCQueryStrategy : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif