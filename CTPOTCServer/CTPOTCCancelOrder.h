/***********************************************************************
 * Module:  CTPOTCCancelOrder.h
 * Author:  milk
 * Modified: 2015年10月14日 0:09:09
 * Purpose: Declaration of the class CTPOTCCancelOrder
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCCancelOrder_h)
#define __CTPOTC_CTPOTCCancelOrder_h

#include "../message/MessageHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCCancelOrder : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif