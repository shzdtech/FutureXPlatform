/***********************************************************************
 * Module:  CTPOTCQueryContractParam.h
 * Author:  milk
 * Modified: 2015年8月23日 9:48:32
 * Purpose: Declaration of the class CTPOTCQueryContractParam
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCQueryContract_h)
#define __CTPOTC_CTPOTCQueryContract_h

#include "../message/MessageHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCQueryContractParam : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif