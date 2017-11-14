#include "CTPTradeWorkerSAProcessor.h"
#include "CTPUtility.h"
#include "CTPConstant.h"
#include "../dataobject/TradeRecordDO.h"
#include "../common/Attribute_Key.h"
#include "../systemsettings/AppContext.h"
#include "../message/DefMessageID.h"
#include "../message/message_macro.h"
#include "../bizutility/ExchangeRouterTable.h"
#include "../bizutility/PositionPortfolioMap.h"

#include "../databaseop/VersionDAO.h"
#include "../databaseop/ContractDAO.h"
#include "../databaseop/TradeDAO.h"
#include "../ordermanager/OrderSeqGen.h"
#include "../ordermanager/OrderPortfolioCache.h"

#include "../bizutility/ContractCache.h"
#include "../litelogger/LiteLogger.h"


CTPTradeWorkerSAProcessor::CTPTradeWorkerSAProcessor(IServerContext* pServerCtx,
	const IPricingDataContext_Ptr& pricingCtx,
	const IUserPositionContext_Ptr& positionCtx)
	: CTPTradeWorkerProcessor(pServerCtx, positionCtx)
{
}

CTPTradeWorkerSAProcessor::~CTPTradeWorkerSAProcessor()
{
	_closing = true;
}

void CTPTradeWorkerSAProcessor::Initialize(IServerContext* pServerCtx)
{
	CTPTradeWorkerProcessor::Initialize(pServerCtx);
}


void CTPTradeWorkerSAProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		if (auto trdDO_Ptr = RefineTrade(pTrade))
		{
			PushToLogQueue(trdDO_Ptr);

			DispatchUserMessage(MSG_ID_TRADE_RTN, 0, trdDO_Ptr->UserID(), trdDO_Ptr);

			if (auto position_ptr = UpdatePosition(trdDO_Ptr))
			{
				DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, trdDO_Ptr->UserID(), position_ptr);
			}

			if (!_loadPositionFromDB)
				QueryUserPositionAsyncIfNeed();
		}
	}
}
