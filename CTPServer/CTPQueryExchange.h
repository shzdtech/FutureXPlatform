/***********************************************************************
 * Module:  CTPQueryExchange.h
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Declaration of the class CTPQueryExchange
 ***********************************************************************/

#if !defined(__CTP_CTPQueryExchange_h)
#define __CTP_CTPQueryExchange_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryExchange : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif