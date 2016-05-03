/***********************************************************************
 * Module:  CTPOTCUpdateStrategy.h
 * Author:  milk
 * Modified: 2015年8月23日 18:51:51
 * Purpose: Declaration of the class CTPOTCUpdateStrategy
 ***********************************************************************/

#if !defined(__CTP_CTPOTCUpdateStrategy_h)
#define __CTP_CTPOTCUpdateStrategy_h

#include "../message/MessageHandler.h"

class CTPOTCUpdateStrategy : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif