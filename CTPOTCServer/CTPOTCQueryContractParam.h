/***********************************************************************
 * Module:  CTPOTCQueryContractParam.h
 * Author:  milk
 * Modified: 2015年8月23日 9:48:32
 * Purpose: Declaration of the class CTPOTCQueryContractParam
 ***********************************************************************/

#if !defined(__CTP_CTPOTCQueryContract_h)
#define __CTP_CTPOTCQueryContract_h

#include "../message/MessageHandler.h"

class CTPOTCQueryContractParam : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif