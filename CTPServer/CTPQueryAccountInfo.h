/***********************************************************************
 * Module:  CTPQueryAccountInfo.h
 * Author:  milk
 * Modified: 2015年7月11日 13:37:12
 * Purpose: Declaration of the class CTPQueryAccountInfo
 ***********************************************************************/

#if !defined(__CTP_CTPQueryAccountInfo_h)
#define __CTP_CTPQueryAccountInfo_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryAccountInfo : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif