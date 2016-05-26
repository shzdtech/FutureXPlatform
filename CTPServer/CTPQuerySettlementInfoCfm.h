/***********************************************************************
 * Module:  CTPQuerySettlementInfoCfm.h
 * Author:  milk
 * Modified: 2015年7月11日 13:38:54
 * Purpose: Declaration of the class CTPQuerySettlementInfoCfm
 ***********************************************************************/

#if !defined(__CTP_CTPQuerySettlementInfoCfm_h)
#define __CTP_CTPQuerySettlementInfoCfm_h

#include "../message/MessageHandler.h"
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPQuerySettlementInfoCfm : public MessageHandler
{
public:
   dataobj_ptr HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session);
   dataobj_ptr HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session);

protected:
private:

};

#endif