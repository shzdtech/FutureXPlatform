#include "CTPPositionUpdated.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"
#include "../message/MessageUtility.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../dataobject/OrderDO.h"
#include "../message/DefMessageID.h"
#include "tradeapi/ThostFtdcTraderApi.h"

dataobj_ptr CTPPositionUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	UserPositionExDO_Ptr ret;

	if (auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0])
	{
		if (pData->PositionDate == THOST_FTDC_PSD_Today)
		{
			ret = CTPUtility::ParseRawPostion(pData);

			if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
			{
				if (pData->Position)
				{
					pWorkerProc->GetUserPositionContext().UpsertPosition(session->getUserInfo()->getUserId(), ret);
				}
				else
				{
					pWorkerProc->GetUserPositionContext().RemovePosition(session->getUserInfo()->getUserId(), ret->InstrumentID(), ret->Direction);
				}
			}
		}
	}

	return ret;
}
