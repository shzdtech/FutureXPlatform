/***********************************************************************
 * Module:  CTPMDTraderProcessor.cpp
 * Author:  milk
 * Modified: 2015年10月24日 13:47:15
 * Purpose: Implementation of the class CTPMDTraderProcessor
 ***********************************************************************/

#include "CTPMDTraderProcessor.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDTraderProcessor::CTPMDTraderProcessor()
// Purpose:    Implementation of CTPMDTraderProcessor::CTPMDTraderProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPMDTraderProcessor::CTPMDTraderProcessor(
	const std::map<std::string, std::string>& configMap,
	std::shared_ptr<CTPMarketDataProcessor> mdProc,
	std::shared_ptr<CTPTradeProcessor> tradeProc)
	: CTPProcessor(configMap), _mdproc(mdProc), _traderproc(tradeProc)
{
	_rawAPI.MdAPI = ((CTPRawAPI*)(mdProc->getRawAPI()))->MdAPI;
	_rawAPI.TrdAPI = ((CTPRawAPI*)(tradeProc->getRawAPI()))->TrdAPI;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPMDTraderProcessor::~CTPMDTraderProcessor()
// Purpose:    Implementation of CTPMDTraderProcessor::~CTPMDTraderProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTPMDTraderProcessor::~CTPMDTraderProcessor()
{
   // TODO : implement
}