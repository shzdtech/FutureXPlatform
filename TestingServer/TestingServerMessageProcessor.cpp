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
#include "../dataobject/MarketDataDO.h"
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

	_mdThread = std::move(std::thread(&TestingServerMessageProcessor::_mdGenerator, this));
}

////////////////////////////////////////////////////////////////////////
// Name:       TestingServerMessageProcessor::~TestingServerMessageProcessor()
// Purpose:    Implementation of TestingServerMessageProcessor::~TestingServerMessageProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

TestingServerMessageProcessor::~TestingServerMessageProcessor()
{
	_exitWorker = true;
	_mdThread.join();
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
		auto pSession = getSession();
		if (auto mdMapPtr = std::static_pointer_cast<MarketDataDOMap>
			(pSession->getContext()->getAttribute(STR_KEY_USER_CONTRACTS)))
		{

			for (auto it = mdMapPtr->begin(); it != mdMapPtr->end(); it++)
			{
				if (std::rand() > std::rand())
				{
					auto& marketDataDO = it->second;
					marketDataDO.BidPrice = std::rand();
					marketDataDO.BidVolume = 1 + std::rand() % 100;
					marketDataDO.AskPrice = marketDataDO.BidPrice + std::rand() % 100;
					marketDataDO.AskVolume = 1 + std::rand() % 100;

					marketDataDO.UpperLimitPrice = marketDataDO.AskPrice * 1.1;
					marketDataDO.LowerLimitPrice = marketDataDO.BidVolume * 0.9;

					OnResponseMacro(MSG_ID_RET_MARKETDATA, &marketDataDO);
				}
			}
		}
	}
}
