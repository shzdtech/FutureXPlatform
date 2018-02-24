#if !defined(__CTP_BIZHANDLERS_H)
#define __CTP_BIZHANDLERS_H

//Market Data
#include "CTPSubMarketData.h"
#include "CTPUnsubMarketData.h"
#include "CTPDepthMarketData.h"
#include "CTPMDLoginHandler.h"
#include "CTPMDLoginSAHandler.h"
#include "CTPSubMarketDataSA.h"
#include "CTPUnsubMarketDataSA.h"

//Trade
#include "CTPTradeLoginHandler.h"
#include "CTPQueryExchange.h"
#include "CTPQueryInstrument.h"
#include "CTPQueryOrder.h"
#include "CTPQueryPosition.h"
#include "CTPQueryTrade.h"
#include "CTPQuerySettlementInfoCfm.h"
#include "CTPQueryAccountInfo.h"
#include "CTPNewOrder.h"
#include "CTPCancelOrder.h"
#include "CTPOrderUpdated.h"
#include "CTPTradeUpdated.h"
#include "CTPTDLoginSA.h"
#include "CTPPositionUpdated.h"
#include "CTPQueryPositionSA.h"
#include "CTPQueryPositionDiffer.h"
#include "CTPSyncPositionDiffer.h"
#include "CTPAddManualTrade.h"
#include "CTPQueryTradeDiff.h"
#include "CTPSyncTrade.h"
#include "CTPQueryPositionPnL.h"

//Transfer
#include "CTPFromBankToFuture.h"
#include "CTPFromFutureToBank.h"
#include "CTPQueryTransferBank.h"
#include "CTPQueryTransferSerial.h"
#include "CTPQueryUserRegAccount.h"
#include "CTPTransferUpdated.h"
#include "CTPQueryUserBankAccount.h"

#endif