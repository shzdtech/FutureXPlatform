#include "CTPTransferUpdated.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"

#include "../dataobject/OrderDO.h"
#include "CTPAPISwitch.h"

dataobj_ptr CTPTransferUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return CTPUtility::ParseRawTransfer((CThostFtdcRspTransferField*)rawRespParams[0]);
}
