#include "CTPOrderUpdated.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"

#include "../dataobject/OrderDO.h"
#include "tradeapi/ThostFtdcTraderApi.h"

dataobj_ptr CTPOrderUpdated::HandleResponse(const uint32_t serialId, param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	dataobj_ptr ret;
	ret = CTPUtility::ParseRawOrder((CThostFtdcOrderField*)rawRespParams[0]);
	ret->SerialId = serialId;

	return ret;
}
