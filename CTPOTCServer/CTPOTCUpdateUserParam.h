/***********************************************************************
 * Module:  CTPOTCUpdateUserParam.h
 * Author:  milk
 * Modified: 2015年8月22日 15:50:10
 * Purpose: Declaration of the class CTPOTCUpdateUserParam
 ***********************************************************************/

#if !defined(__CTP_CTPOTCUpdateUserParam_h)
#define __CTP_CTPOTCUpdateUserParam_h

#include "../message/MessageHandler.h"

class CTPOTCUpdateUserParam : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif