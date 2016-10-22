#include "CTPTransferUpdated.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"

#include "../dataobject/OrderDO.h"
#include "tradeapi/ThostFtdcTraderApi.h"

dataobj_ptr CTPTransferUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	return CTPUtility::ParseRawTransfer((CThostFtdcRspTransferField*)rawRespParams[0]);
}
