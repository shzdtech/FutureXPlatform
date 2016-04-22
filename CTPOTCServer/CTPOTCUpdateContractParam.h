/***********************************************************************
 * Module:  CTPOTCUpdateContractParam.h
 * Author:  milk
 * Modified: 2015年8月22日 15:50:23
 * Purpose: Declaration of the class CTPOTCUpdateContractParam
 ***********************************************************************/

#if !defined(__CTP_CTPOTCUpdateContractParam_h)
#define __CTP_CTPOTCUpdateContractParam_h

#include "../message/MessageHandler.h"

class CTPOTCUpdateContractParam : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif