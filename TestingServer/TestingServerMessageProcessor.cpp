/***********************************************************************
 * Module:  TestingServerMessageProcessor.cpp
 * Author:  milk
 * Modified: 2016年4月29日 19:54:45
 * Purpose: Implementation of the class TestingServerMessageProcessor
 ***********************************************************************/

#include "TestingServerMessageProcessor.h"

#include "../message/DefMessageID.h"
#include "../message/message_macro.h"

#include "../common/Attribute_Key.h"
#include "../dataobject/MarketDataDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/TypedefDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       TestingServerMessageProcessor::TestingServerMessageProcessor()
 // Purpose:    Implementation of TestingServerMessageProcessor::TestingServerMessageProcessor()
 // Return:     
 ////////////////////////////////////////////////////////////////////////

TestingServerMessageProcessor::TestingServerMessageProcessor(const std::map<std::string, std::string>& configMap)
{
	auto it = configMap.find("mdInterval");
	if (it != configMap.end())
	{
		_mdGenInterval = std::chrono::milliseconds(std::stoi(it->second, nullptr, 0));
	}

	_mdThread = std::async(std::launch::async, &TestingServerMessageProcessor::_mdGenerator, this);
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingServerMessageProcessor::~TestingServerMessageProcessor()
// Purpose:    Implementation of TestingServerMessageProcessor::~TestingServerMessageProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

TestingServerMessageProcessor::~TestingServerMessageProcessor()
{
}

bool TestingServerMessageProcessor::OnSessionClosing(void)
{
	_exitWorker = true;
	try
	{
		if (_mdThread.valid())
			_mdThread.wait_for(std::chrono::seconds(10));
	}
	catch (...)
	{
	}
	
	return true;
}

IRawAPI * TestingServerMessageProcessor::getRawAPI(void)
{
	return nullptr;
}

void TestingServerMessageProcessor::_mdGenerator()
{
	while (!_exitWorker)
	{
		std::this_thread::sleep_for(_mdGenInterval);
		if (!_exitWorker)
			if (auto session_ptr = getMessageSession())
			{
				if (auto mdMapPtr = std::static_pointer_cast<MarketDataDOMap>
					(session_ptr->getContext()->getAttribute(STR_KEY_USER_CONTRACTS)))
				{
					auto table = mdMapPtr->lock_table();
					for (auto it = table.begin(); it != table.end(); it++)
					{
						if (std::rand() > std::rand())
						{
							auto& marketDataDO = it->second;
							GenRandomMD(marketDataDO);
						}
					}
				}
			}
	}
}

void TestingServerMessageProcessor::GenRandomMD(MarketDataDO& marketDataDO)
{
	marketDataDO.Bid().Price = std::rand();
	marketDataDO.Bid().Volume = 1 + std::rand() % 100;
	marketDataDO.Ask().Price = marketDataDO.Bid().Price + std::rand() % 100;
	marketDataDO.Ask().Volume = 1 + std::rand() % 100;
	marketDataDO.LastPrice = (marketDataDO.Bid().Price + marketDataDO.Ask().Price) / 2;
	marketDataDO.Volume += std::rand() % 5;
	marketDataDO.UpperLimitPrice = marketDataDO.Ask().Price * 1.1;
	marketDataDO.LowerLimitPrice = marketDataDO.Bid().Volume * 0.9;
	marketDataDO.PreSettlementPrice = std::rand();
	marketDataDO.HighestPrice = marketDataDO.Ask().Price * 1.09;
	marketDataDO.LowestPrice = marketDataDO.Bid().Price * 0.91;
	marketDataDO.OpenPrice = marketDataDO.LastPrice;
	auto tm = std::time(nullptr);
	auto ptm = std::localtime(&tm);
	marketDataDO.UpdateTime = ptm->tm_hour * 3600 + ptm->tm_min * 60 + ptm->tm_sec;
	marketDataDO.TradingDay = (ptm->tm_year + 1900) * 10000 + (ptm->tm_mon + 1) * 100 + ptm->tm_mday;

	OnResponseMacro(MSG_ID_RET_MARKETDATA, 0, &marketDataDO);
	OnResponseMacro(MSG_ID_RTN_PRICING, 0, &marketDataDO);
}
