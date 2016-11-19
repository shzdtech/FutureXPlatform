#include "CTPPositionUpdated.h"
#include "CTPUtility.h"
#include "CTPMapping.h"
#include "CTPConstant.h"
#include "../message/MessageUtility.h"
#include "CTPTradeWorkerProcessor.h"
#include "CTPWorkerProcessorID.h"

#include "../litelogger/LiteLogger.h"

#include "../dataobject/OrderDO.h"
#include "../message/DefMessageID.h"
#include "tradeapi/ThostFtdcTraderApi.h"

dataobj_ptr CTPPositionUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	UserPositionExDO_Ptr ret;

	if (auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(session->getProcessor()))
		{
			ret = CTPUtility::ParseRawPosition(pData);

			LOG_DEBUG << pData->InstrumentID << ',' << pData->PositionDate << ',' << pData->PosiDirection;

			if (ret->ExchangeID() == EXCHANGE_SHFE)
			{
				auto position_ptr = pWorkerProc->GetUserPositionContext().GetPosition(session->getUserInfo()->getUserId(), ret->InstrumentID(), ret->Direction);
				if (position_ptr)
				{
					if (pData->PositionDate == THOST_FTDC_PSD_Today)
					{
						ret->YdPosition = position_ptr->YdPosition;
						ret->YdCost = position_ptr->YdCost;
						ret->YdProfit = position_ptr->YdProfit;
					}
					else
					{
						position_ptr->YdPosition = ret->YdPosition;
						position_ptr->YdCost = ret->YdCost;
						position_ptr->YdProfit = ret->YdProfit;
						ret = position_ptr;
					}
				}
				else
				{
					ret->PositionDateFlag = PositionDateFlagType::PSD_TODAY;
				}
			}

			if (ret->Position())
			{
				pWorkerProc->GetUserPositionContext().UpsertPosition(session->getUserInfo()->getUserId(), ret);
			}
			else
			{
				if (!pWorkerProc->GetUserPositionContext().RemovePosition(session->getUserInfo()->getUserId(), ret->InstrumentID(), ret->Direction))
				{
					ret.reset();
				}
			}
		}
	}

	return ret;
}
