/***********************************************************************
 * Module:  CTPQueryPosition.h
 * Author:  milk
 * Modified: 2015年7月11日 13:38:08
 * Purpose: Declaration of the class CTPQueryPosition
 ***********************************************************************/

#if !defined(__CTP_CTPQueryPosition_h)
#define __CTP_CTPQueryPosition_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryPosition : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(ParamVector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif