/***********************************************************************
 * Module:  CTPOTCUpdateUserParam.h
 * Author:  milk
 * Modified: 2015年8月22日 15:50:10
 * Purpose: Declaration of the class CTPOTCUpdateUserParam
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCUpdateUserParam_h)
#define __CTPOTC_CTPOTCUpdateUserParam_h

#include "../message/MessageHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCUpdateUserParam : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif