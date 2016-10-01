/***********************************************************************
 * Module:  CTPSimMarketData.cpp
 * Author:  milk
 * Modified: 2015年3月6日 20:17:29
 * Purpose: Implementation of the class CTPSimMarketData
 ***********************************************************************/

#include "CTPSimMarketData.h"
#include "../CTPServer/tradeapi/ThostFtdcMdApi.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/TemplateDO.h"
#include "../message/MessageUtility.h"
#include "../message/BizError.h"
#include <boost/locale/encoding.hpp>

#include "CTPOTCWorkerProcessor.h"

dataobj_ptr CTPSimMarketData::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	MarketDataDO* pMdO = (MarketDataDO*)reqDO.get();
	if (auto wkProcPtr = MessageUtility::WorkerProcessorPtr<CTPOTCWorkerProcessor>(session->getProcessor()))
	{
		CThostFtdcDepthMarketDataField md{};
		std::strcpy(md.ExchangeID, pMdO->ExchangeID().data());
		std::strcpy(md.InstrumentID, pMdO->InstrumentID().data());
		md.BidPrice1 = pMdO->Bid().Price;
		md.BidVolume1 = pMdO->Bid().Volume;
		md.AskPrice1 = pMdO->Ask().Price;
		md.AskVolume1 = pMdO->Ask().Volume;

		wkProcPtr->OnRtnDepthMarketData(&md);
	}

	return nullptr;
}
