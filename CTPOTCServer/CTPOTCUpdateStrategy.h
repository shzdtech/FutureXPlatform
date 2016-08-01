/***********************************************************************
 * Module:  CTPOTCUpdateStrategy.h
 * Author:  milk
 * Modified: 2015年8月23日 18:51:51
 * Purpose: Declaration of the class CTPOTCUpdateStrategy
 ***********************************************************************/

#if !defined(__CTPOTC_CTPOTCUpdateStrategy_h)
#define __CTPOTC_CTPOTCUpdateStrategy_h

#include "../message/MessageHandler.h"
#include "ctpotc_export.h"

class CTP_OTC_CLASS_EXPORT CTPOTCUpdateStrategy : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif