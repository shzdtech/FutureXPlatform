/***********************************************************************
 * Module:  CTPQueryTransferBank.h
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Declaration of the class CTPQueryTransferBank
 ***********************************************************************/

#if !defined(__CTP_CTPQueryTransferBank_h)
#define __CTP_CTPQueryTransferBank_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryTransferBank : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif