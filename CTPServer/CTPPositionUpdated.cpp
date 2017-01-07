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

dataobj_ptr CTPPositionUpdated::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	UserPositionExDO_Ptr ret;

	if (auto pData = (CThostFtdcInvestorPositionField*)rawRespParams[0])
	{
		if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPTradeWorkerProcessor>(msgProcessor))
		{
			ret = CTPUtility::ParseRawPosition(pData);

			LOG_DEBUG << pData->InstrumentID << ',' << pData->PositionDate << ',' << pData->PosiDirection;

			if (ret->ExchangeID() == EXCHANGE_SHFE)
			{
				auto position_ptr = pWorkerProc->GetUserPositionContext().GetPosition(session->getUserInfo().getUserId(), ret->InstrumentID(), ret->Direction);
				if (pData->PositionDate == THOST_FTDC_PSD_Today)
				{
					if (position_ptr)
					{
						//ret->YdPosition = position_ptr->YdPosition;
						ret->YdInitPosition = position_ptr->YdInitPosition;
						ret->YdCost = position_ptr->YdCost;
						ret->YdProfit = position_ptr->YdProfit;
						pWorkerProc->GetUserPositionContext().UpsertPosition(session->getUserInfo().getUserId(), *ret);
					}
				}
				else
				{
					if (position_ptr)
					{
						//position_ptr->YdPosition = ret->YdPosition;
						position_ptr->YdInitPosition = ret->YdInitPosition;
						position_ptr->YdCost = ret->YdCost;
						position_ptr->YdProfit = ret->YdProfit;
					}
					else
					{
						position_ptr = ret;
					}
					pWorkerProc->GetUserPositionContext().UpsertPosition(session->getUserInfo().getUserId(), *position_ptr);
				}
			}
			else
				pWorkerProc->GetUserPositionContext().UpsertPosition(session->getUserInfo().getUserId(), *ret, UserPositionContext::ADJUST_HISTORY);

			if(ret = pWorkerProc->GetUserPositionContext().GetPosition(session->getUserInfo().getUserId(), ret->InstrumentID(), ret->Direction))
				if(ret->Position() == 0)
					ret.reset();
		}
	}

	return ret;
}
