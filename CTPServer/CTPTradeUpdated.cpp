#include "CTPTradeUpdated.h"
#include "CTPUtility.h"

dataobj_ptr CTPTradeUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	dataobj_ptr ret;
	return CTPUtility::ParseRawTrade((CThostFtdcTradeField*)rawRespParams[0]);
}
