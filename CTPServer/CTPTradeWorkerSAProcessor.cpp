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
	if (_tradeDBSerializer.joinable())
		_tradeDBSerializer.join();
}

void CTPTradeWorkerSAProcessor::Initialize(IServerContext* pServerCtx)
{
	if (_logTrades)
		_tradeDBSerializer = std::move(std::thread(&CTPTradeWorkerSAProcessor::LogTrade, this));

	CTPTradeWorkerProcessor::Initialize(pServerCtx);
}

void CTPTradeWorkerSAProcessor::LogTrade()
{
	while (!_closing)
	{
		bool hasError = false;
		TradeRecordDO_Ptr trade_ptr;
		if (_tradeQueue.pop(trade_ptr))
		{
			if (trade_ptr)
			{
				try
				{
					TradeDAO::SaveExchangeTrade(*trade_ptr);
				}
				catch (std::exception& ex)
				{
					hasError = true;
					LOG_ERROR << ex.what();
				}
				catch (...)
				{
					hasError = true;
				}

				if (hasError)
				{
					_tradeQueue.push(trade_ptr);
				}
			}
		}

		if (hasError || _tradeQueue.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
}

void CTPTradeWorkerSAProcessor::OnRtnTrade(CThostFtdcTradeField * pTrade)
{
	if (pTrade)
	{
		if (auto trdDO_Ptr = RefineTrade(pTrade))
		{
			if (_logTrades)
				_tradeQueue.push(trdDO_Ptr);

			DispatchUserMessage(MSG_ID_TRADE_RTN, 0, trdDO_Ptr->UserID(), trdDO_Ptr);

			if (auto position_ptr = UpdatePosition(trdDO_Ptr))
			{
				DispatchUserMessage(MSG_ID_POSITION_UPDATED, 0, trdDO_Ptr->UserID(), position_ptr);
			}

			// Try update position
			if (!_exiting && !_updateFlag.test_and_set(std::memory_order::memory_order_acquire))
			{
				_updateTask = std::async(std::launch::async, &CTPTradeProcessor::QueryPositionAsync, this);
			}
		}
	}
}
