#include "CTPTradeUpdated.h"
#include "CTPUtility.h"

dataobj_ptr CTPTradeUpdated::HandleResponse(const uint32_t serialId, param_vector & rawRespParams, IRawAPI * rawAPI, ISession * session)
{
	dataobj_ptr ret;
	if (ret = CTPUtility::ParseRawTrade((CThostFtdcTradeField*)rawRespParams[0]))
	{
		ret->SerialId = serialId;
	}

	return ret;
}
