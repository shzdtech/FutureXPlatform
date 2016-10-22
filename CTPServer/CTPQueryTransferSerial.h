/***********************************************************************
 * Module:  CTPQueryTransferSerial.h
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Declaration of the class CTPQueryTransferSerial
 ***********************************************************************/

#if !defined(__CTP_CTPQueryTransferSerial_h)
#define __CTP_CTPQueryTransferSerial_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQueryTransferSerial : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif